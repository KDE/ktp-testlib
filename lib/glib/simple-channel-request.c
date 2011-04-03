/*
 * simple-channel-request.c - simple channel request service.
 *
 * Copyright © 2010 Collabora Ltd.
 *
 * Copying and distribution of this file, with or without modification,
 * are permitted in any medium without royalty provided the copyright
 * notice and this notice are preserved.
 */

#include "simple-channel-request.h"

#include <telepathy-glib/client.h>
#include <telepathy-glib/interfaces.h>
#include <telepathy-glib/proxy-subclass.h>
#include <telepathy-glib/svc-channel-request.h>
#include <telepathy-glib/svc-generic.h>
#include <telepathy-glib/telepathy-glib.h>
#include <telepathy-glib/util.h>

#include "util.h"
#include "textchan-null.h"

static void channel_request_iface_init (gpointer, gpointer);

G_DEFINE_TYPE_WITH_CODE (TpTestsSimpleChannelRequest,
    tp_tests_simple_channel_request,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_CHANNEL_REQUEST,
        channel_request_iface_init);
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_DBUS_PROPERTIES,
        tp_dbus_properties_mixin_iface_init)
    )


/* TP_IFACE_CHANNEL_REQUEST is implied */
static const char *CHANNEL_REQUEST_INTERFACES[] = { NULL };

enum
{
  PROP_0,
  PROP_PATH,
  PROP_ACCOUNT,
  PROP_USER_ACTION_TIME,
  PROP_PREFERRED_HANDLER,
  PROP_REQUESTS,
  PROP_CONNECTION,
  PROP_INTERFACES,
};

struct _TpTestsSimpleChannelRequestPrivate
{
  /* D-Bus properties */
  gchar *account_path;
  gint64 user_action_time;
  gchar *preferred_handler;
  GPtrArray *requests;

  /* Our own path */
  gchar *path;

  /* connection used to create channels */
  TpTestsSimpleConnection *conn;
};

static void
handle_channels_cb (TpClient *client,
    const GError *error,
    gpointer user_data,
    GObject *weak_object)
{
  TpTestsSimpleChannelRequest *self = SIMPLE_CHANNEL_REQUEST (weak_object);

  if (error != NULL)
    {
      tp_svc_channel_request_emit_failed (self,
          tp_error_get_dbus_name (error->code), error->message);
      return;
    }

  tp_svc_channel_request_emit_succeeded (self);
}

static void
add_channel (TpTestsSimpleChannelRequest *self,
    GPtrArray *channels)
{
  const gchar *target_id;
  gchar *chan_path;
  GHashTable *request;
  GHashTable *props;

  request = g_ptr_array_index (self->priv->requests, 0);
  target_id = tp_asv_get_string (request, TP_PROP_CHANNEL_TARGET_ID);
  g_assert (target_id != NULL);

  chan_path = tp_tests_simple_connection_ensure_text_chan (self->priv->conn,
      target_id, &props);

  g_ptr_array_add (channels, tp_value_array_build (2,
      DBUS_TYPE_G_OBJECT_PATH, chan_path,
      TP_HASH_TYPE_STRING_VARIANT_MAP, props,
      G_TYPE_INVALID));

  g_free (chan_path);
  g_hash_table_unref (props);
}

static void
free_channel_details (gpointer data,
    gpointer user_data)
{
  g_boxed_free (TP_STRUCT_TYPE_CHANNEL_DETAILS, data);
}

static void
tp_tests_simple_channel_request_proceed (TpSvcChannelRequest *request,
    DBusGMethodInvocation *context)
{
  TpTestsSimpleChannelRequest *self = SIMPLE_CHANNEL_REQUEST (request);
  TpClient *client;
  TpDBusDaemon *dbus;
  gchar *client_path;
  GPtrArray *channels;
  GPtrArray *satisfied;
  GHashTable *info;
  TpBaseConnection *base_conn = (TpBaseConnection *) self->priv->conn;
  GHashTable *req;

  req = g_ptr_array_index (self->priv->requests, 0);
  g_assert (req != NULL);
  if (tp_asv_get_boolean (req, "ProceedFail", NULL))
    {
      /* We have been asked to fail */
     GError error = { TP_ERRORS, TP_ERROR_INVALID_ARGUMENT,
          "Computer says no" };

      dbus_g_method_return_error (context, &error);
      return;
    }

  tp_svc_channel_request_return_from_proceed (context);

  if (tp_asv_get_boolean (req, "FireFailed", NULL))
    {
      /* We have been asked to fire the 'Failed' signal */
      tp_svc_channel_request_emit_failed (self, TP_ERROR_STR_INVALID_ARGUMENT,
          "Let's fail!");
      return;
    }

  /* We just support handling request having a preferred handler */
  if (self->priv->preferred_handler == NULL ||
      !tp_strdiff (self->priv->preferred_handler, ""))
    {
      tp_svc_channel_request_emit_failed (self, TP_ERROR_STR_NOT_AVAILABLE,
          "ChannelRequest doesn't have a preferred handler");
      return;
    }

  if (!tp_strdiff (self->priv->preferred_handler, "Fake"))
    {
      /* Pretend that the channel has been handled */
      tp_svc_channel_request_emit_succeeded (self);
      return;
    }

  /* Call HandleChannels() on the preferred handler */
  client_path = g_strdelimit (g_strdup_printf ("/%s",
        self->priv->preferred_handler), ".", '/');

  dbus = tp_dbus_daemon_dup (NULL);
  g_assert (dbus != NULL);

  client = tp_tests_object_new_static_class (TP_TYPE_CLIENT,
          "dbus-daemon", dbus,
          "bus-name", self->priv->preferred_handler,
          "object-path", client_path,
          NULL);

  tp_proxy_add_interface_by_id (TP_PROXY (client), TP_IFACE_QUARK_CLIENT);
  tp_proxy_add_interface_by_id (TP_PROXY (client),
      TP_IFACE_QUARK_CLIENT_HANDLER);

  channels = g_ptr_array_sized_new (1);
  add_channel (self, channels);

  satisfied = g_ptr_array_sized_new (1);
  g_ptr_array_add (satisfied, self->priv->path);

  info = g_hash_table_new (NULL, NULL);

  tp_cli_client_handler_call_handle_channels (client, -1,
      self->priv->account_path, base_conn->object_path, channels,
      satisfied, self->priv->user_action_time, info, handle_channels_cb, NULL,
      NULL, G_OBJECT (self));

  g_free (client_path);
  g_ptr_array_foreach (channels, free_channel_details, NULL);
  g_ptr_array_free (channels, TRUE);
  g_ptr_array_free (satisfied, TRUE);
  g_hash_table_unref (info);
  g_object_unref (dbus);
  g_object_unref (client);
}

static void
tp_tests_simple_channel_request_cancel (TpSvcChannelRequest *request,
    DBusGMethodInvocation *context)
{
  tp_svc_channel_request_emit_failed (request, TP_ERROR_STR_CANCELLED,
      "ChannelRequest has been cancelled");

  tp_svc_channel_request_return_from_cancel (context);
}

static void
channel_request_iface_init (gpointer klass,
    gpointer unused G_GNUC_UNUSED)
{
#define IMPLEMENT(x) tp_svc_channel_request_implement_##x (\
  klass, tp_tests_simple_channel_request_##x)
  IMPLEMENT (proceed);
  IMPLEMENT (cancel);
#undef IMPLEMENT
}

static void
tp_tests_simple_channel_request_init (TpTestsSimpleChannelRequest *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
      TP_TESTS_TYPE_SIMPLE_CHANNEL_REQUEST,
      TpTestsSimpleChannelRequestPrivate);
}

static void
tp_tests_simple_channel_request_get_property (GObject *object,
              guint property_id,
              GValue *value,
              GParamSpec *spec)
{
  TpTestsSimpleChannelRequest *self = SIMPLE_CHANNEL_REQUEST (object);

  switch (property_id) {
    case PROP_PATH:
      g_value_set_string (value, self->priv->path);
      break;

    case PROP_ACCOUNT:
      g_value_set_string (value, self->priv->account_path);
      break;

    case PROP_USER_ACTION_TIME:
      g_value_set_int64 (value, self->priv->user_action_time);
      break;

    case PROP_PREFERRED_HANDLER:
      g_value_set_string (value, self->priv->preferred_handler);
      break;

    case PROP_REQUESTS:
      g_value_set_boxed (value, self->priv->requests);
      break;

    case PROP_INTERFACES:
      g_value_set_boxed (value, CHANNEL_REQUEST_INTERFACES);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, spec);
      break;
  }
}

static void
tp_tests_simple_channel_request_set_property (GObject *object,
              guint property_id,
              const GValue *value,
              GParamSpec *spec)
{
  TpTestsSimpleChannelRequest *self = SIMPLE_CHANNEL_REQUEST (object);

  switch (property_id) {
    case PROP_PATH:
      self->priv->path = g_value_dup_string (value);
      break;

    case PROP_ACCOUNT:
      self->priv->account_path = g_value_dup_string (value);
      break;

    case PROP_USER_ACTION_TIME:
      self->priv->user_action_time = g_value_get_int64 (value);
      break;

    case PROP_PREFERRED_HANDLER:
      self->priv->preferred_handler = g_value_dup_string (value);
      break;

    case PROP_REQUESTS:
      self->priv->requests = g_value_dup_boxed (value);
      break;

    case PROP_CONNECTION:
      self->priv->conn = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, spec);
      break;
  }
}

static void
tp_tests_simple_channel_request_dispose (GObject *object)
{
  TpTestsSimpleChannelRequest *self = SIMPLE_CHANNEL_REQUEST (object);

  g_free (self->priv->path);
  g_free (self->priv->account_path);
  g_free (self->priv->preferred_handler);

  if (self->priv->requests != NULL)
    {
      g_boxed_free (TP_ARRAY_TYPE_CHANNEL_CLASS_LIST, self->priv->requests);
      self->priv->requests = NULL;
    }

  tp_clear_object (&self->priv->conn);

  if (G_OBJECT_CLASS (tp_tests_simple_channel_request_parent_class)->dispose != NULL)
    G_OBJECT_CLASS (tp_tests_simple_channel_request_parent_class)->dispose (object);
}

static void
tp_tests_simple_channel_request_class_init (
    TpTestsSimpleChannelRequestClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;
  GParamSpec *param_spec;

  static TpDBusPropertiesMixinPropImpl am_props[] = {
        { "Interfaces", "interfaces", NULL },
        { NULL }
  };

  static TpDBusPropertiesMixinIfaceImpl prop_interfaces[] = {
        { TP_IFACE_CHANNEL_REQUEST,
          tp_dbus_properties_mixin_getter_gobject_properties,
          NULL,
          am_props
        },
        { NULL },
  };

  g_type_class_add_private (klass, sizeof (TpTestsSimpleChannelRequestPrivate));
  object_class->get_property = tp_tests_simple_channel_request_get_property;
  object_class->set_property = tp_tests_simple_channel_request_set_property;

  object_class->dispose = tp_tests_simple_channel_request_dispose;

  param_spec = g_param_spec_string ("path", "Path",
      "Path of this ChannelRequest",
      NULL,
      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_PATH, param_spec);

  param_spec = g_param_spec_string ("account", "Account",
      "Path of the Account",
      NULL,
      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_ACCOUNT, param_spec);

  param_spec = g_param_spec_int64 ("user-action-time", "UserActionTime",
      "UserActionTime",
      G_MININT64, G_MAXINT64, 0,
      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_USER_ACTION_TIME,
      param_spec);

  param_spec = g_param_spec_string ("preferred-handler", "PreferredHandler",
      "PreferredHandler",
      NULL,
      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_PREFERRED_HANDLER,
      param_spec);

  param_spec = g_param_spec_boxed ("requests", "Requests",
      "Requests",
      TP_ARRAY_TYPE_CHANNEL_CLASS_LIST,
      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_REQUESTS, param_spec);

  param_spec = g_param_spec_boxed ("interfaces", "Extra D-Bus interfaces",
      "In this case we only implement ChannelRequest, so none.",
      G_TYPE_STRV,
      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_INTERFACES, param_spec);

  param_spec = g_param_spec_object ("connection", "TpBaseConnection",
      "connection to use when creating channels",
      TP_TYPE_BASE_CONNECTION,
      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_CONNECTION, param_spec);

  klass->dbus_props_class.interfaces = prop_interfaces;
  tp_dbus_properties_mixin_class_init (object_class,
      G_STRUCT_OFFSET (TpTestsSimpleChannelRequestClass, dbus_props_class));
}

TpTestsSimpleChannelRequest *
tp_tests_simple_channel_request_new (const gchar *path,
    TpTestsSimpleConnection *conn,
    const gchar *account_path,
    gint64 user_action_time,
    const gchar *preferred_handler,
    GPtrArray *requests)
{
  return tp_tests_object_new_static_class (
      TP_TESTS_TYPE_SIMPLE_CHANNEL_REQUEST,
      "path", path,
      "connection", conn,
      "account", account_path,
      "user-action-time", user_action_time,
      "preferred-handler", preferred_handler,
      "requests", requests,
      NULL);
}
