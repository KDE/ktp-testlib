/*
 * simple-channel-dispatcher.c - simple channel dispatcher service.
 *
 * Copyright © 2010 Collabora Ltd.
 *
 * Copying and distribution of this file, with or without modification,
 * are permitted in any medium without royalty provided the copyright
 * notice and this notice are preserved.
 */

#include "simple-channel-dispatcher.h"

#include <telepathy-glib/base-connection.h>
#include <telepathy-glib/dbus.h>
#include <telepathy-glib/gtypes.h>
#include <telepathy-glib/interfaces.h>
#include <telepathy-glib/svc-generic.h>
#include <telepathy-glib/svc-channel-dispatcher.h>
#include <telepathy-glib/util.h>

#include "simple-channel-request.h"
#include "simple-conn.h"

static void channel_dispatcher_iface_init (gpointer, gpointer);

G_DEFINE_TYPE_WITH_CODE (TpTestsSimpleChannelDispatcher,
    tp_tests_simple_channel_dispatcher,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_CHANNEL_DISPATCHER,
        channel_dispatcher_iface_init);
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_DBUS_PROPERTIES,
        tp_dbus_properties_mixin_iface_init)
    )

/* signals */
enum {
  CHANNEL_REQUEST_CREATED,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

/* TP_IFACE_CHANNEL_DISPATCHER is implied */
static const char *CHANNEL_DISPATCHER_INTERFACES[] = { NULL };

enum
{
  PROP_0,
  PROP_INTERFACES,
  PROP_CONNECTION,
};

struct _TpTestsSimpleChannelDispatcherPrivate
{
  /* To keep things simpler, this CD can only create channels using one
   * connection */
  TpTestsSimpleConnection *conn;

  /* List of reffed TpTestsSimpleChannelRequest */
  GSList *requests;

  /* Used when ensuring a channel to store its handler.
   * If this is set we fake that the channel already exist and re-call
   * HandleChannels() on the handler rather than creating a new channel.
   * This is pretty stupid but good enough for our tests. */
  gchar *old_handler;
};

static gchar *
create_channel_request (TpTestsSimpleChannelDispatcher *self,
    const gchar *account,
    GHashTable *request,
    gint64 user_action_time,
    const gchar *preferred_handler)
{
  TpTestsSimpleChannelRequest *chan_request;
  GPtrArray *requests;
  static guint count = 0;
  gchar *path;
  TpDBusDaemon *dbus;

  requests = g_ptr_array_sized_new (1);
  g_ptr_array_add (requests, request);

  path = g_strdup_printf ("/Request%u", count++);

  chan_request = tp_tests_simple_channel_request_new (path,
      self->priv->conn, account, user_action_time, preferred_handler, requests);

  self->priv->requests = g_slist_append (self->priv->requests, chan_request);

  g_ptr_array_free (requests, TRUE);

  dbus = tp_dbus_daemon_dup (NULL);
  g_assert (dbus != NULL);

  tp_dbus_daemon_register_object (dbus, path, chan_request);

  g_object_unref (dbus);

  g_signal_emit (self, signals[CHANNEL_REQUEST_CREATED], 0, chan_request);

  return path;
}

static void
tp_tests_simple_channel_dispatcher_create_channel (
    TpSvcChannelDispatcher *dispatcher,
    const gchar *account,
    GHashTable *request,
    gint64 user_action_time,
    const gchar *preferred_handler,
    DBusGMethodInvocation *context)
{
  TpTestsSimpleChannelDispatcher *self = SIMPLE_CHANNEL_DISPATCHER (dispatcher);
  gchar *path;

  if (tp_asv_get_boolean (request, "CreateChannelFail", NULL))
    {
      /* Fail to create the channel */
      GError error = { TP_ERRORS, TP_ERROR_INVALID_ARGUMENT,
          "Computer says no" };

      dbus_g_method_return_error (context, &error);
      return;
    }

  path = create_channel_request (self, account, request, user_action_time,
      preferred_handler);

  tp_svc_channel_dispatcher_return_from_create_channel (context, path);

  g_free (path);
}

static void
tp_tests_simple_channel_dispatcher_ensure_channel (
    TpSvcChannelDispatcher *dispatcher,
    const gchar *account,
    GHashTable *request,
    gint64 user_action_time,
    const gchar *preferred_handler,
    DBusGMethodInvocation *context)
{
  TpTestsSimpleChannelDispatcher *self = SIMPLE_CHANNEL_DISPATCHER (dispatcher);
  gchar *path;

  if (self->priv->old_handler != NULL)
    {
      /* Pretend that the channel already exists */
      path = create_channel_request (self, account, request, user_action_time,
          self->priv->old_handler);
    }
  else
    {
      path = create_channel_request (self, account, request, user_action_time,
          preferred_handler);

      self->priv->old_handler = g_strdup (preferred_handler);
    }

  tp_svc_channel_dispatcher_return_from_ensure_channel (context, path);

  g_free (path);
}

static void
channel_dispatcher_iface_init (gpointer klass,
    gpointer unused G_GNUC_UNUSED)
{
#define IMPLEMENT(x) tp_svc_channel_dispatcher_implement_##x (\
  klass, tp_tests_simple_channel_dispatcher_##x)
  IMPLEMENT (create_channel);
  IMPLEMENT (ensure_channel);
#undef IMPLEMENT
}


static void
tp_tests_simple_channel_dispatcher_init (TpTestsSimpleChannelDispatcher *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
      TP_TESTS_TYPE_SIMPLE_CHANNEL_DISPATCHER,
      TpTestsSimpleChannelDispatcherPrivate);
}

static void
tp_tests_simple_channel_dispatcher_get_property (GObject *object,
              guint property_id,
              GValue *value,
              GParamSpec *spec)
{
  switch (property_id) {
    case PROP_INTERFACES:
      g_value_set_boxed (value, CHANNEL_DISPATCHER_INTERFACES);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, spec);
      break;
  }
}

static void
tp_tests_simple_channel_dispatcher_set_property (GObject *object,
              guint property_id,
              const GValue *value,
              GParamSpec *spec)
{
  TpTestsSimpleChannelDispatcher *self = SIMPLE_CHANNEL_DISPATCHER (object);

  switch (property_id) {
    case PROP_CONNECTION:
      self->priv->conn = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, spec);
      break;
  }
}

static void
tp_tests_simple_channel_dispatcher_dispose (GObject *object)
{
  TpTestsSimpleChannelDispatcher *self = SIMPLE_CHANNEL_DISPATCHER (object);

  tp_clear_object (&self->priv->conn);

  g_slist_foreach (self->priv->requests, (GFunc) g_object_unref, NULL);
  g_slist_free (self->priv->requests);

  g_free (self->priv->old_handler);

  if (G_OBJECT_CLASS (tp_tests_simple_channel_dispatcher_parent_class)->dispose != NULL)
    G_OBJECT_CLASS (tp_tests_simple_channel_dispatcher_parent_class)->dispose (object);
}

static void
tp_tests_simple_channel_dispatcher_class_init (
    TpTestsSimpleChannelDispatcherClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;
  GParamSpec *param_spec;

  static TpDBusPropertiesMixinPropImpl am_props[] = {
        { "Interfaces", "interfaces", NULL },
        { NULL }
  };

  static TpDBusPropertiesMixinIfaceImpl prop_interfaces[] = {
        { TP_IFACE_CHANNEL_DISPATCHER,
          tp_dbus_properties_mixin_getter_gobject_properties,
          NULL,
          am_props
        },
        { NULL },
  };

  g_type_class_add_private (klass, sizeof (TpTestsSimpleChannelDispatcherPrivate));
  object_class->get_property = tp_tests_simple_channel_dispatcher_get_property;
  object_class->set_property = tp_tests_simple_channel_dispatcher_set_property;

  object_class->dispose = tp_tests_simple_channel_dispatcher_dispose;

  param_spec = g_param_spec_boxed ("interfaces", "Extra D-Bus interfaces",
      "In this case we only implement ChannelDispatcher, so none.",
      G_TYPE_STRV,
      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_INTERFACES, param_spec);

  param_spec = g_param_spec_object ("connection", "TpTestsSimpleConnection",
      "connection to use when creating channels",
      TP_TESTS_TYPE_SIMPLE_CONNECTION,
      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_CONNECTION, param_spec);

  /* Fired when we create a new channel request object. This can be used in
   * test to track the progression of a request. */
  signals[CHANNEL_REQUEST_CREATED] = g_signal_new ("channel-request-created",
      G_TYPE_FROM_CLASS (object_class),
      G_SIGNAL_RUN_LAST,
      0, NULL, NULL,
      g_cclosure_marshal_VOID__OBJECT,
      G_TYPE_NONE, 1, TP_TESTS_TYPE_SIMPLE_CHANNEL_REQUEST);

  klass->dbus_props_class.interfaces = prop_interfaces;
  tp_dbus_properties_mixin_class_init (object_class,
      G_STRUCT_OFFSET (TpTestsSimpleChannelDispatcherClass, dbus_props_class));
}
