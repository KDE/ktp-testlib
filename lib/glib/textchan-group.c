/*
 * a stub anonymous MUC
 *
 * Copyright (C) 2008 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2008 Nokia Corporation
 *
 * Copying and distribution of this file, with or without modification,
 * are permitted in any medium without royalty provided the copyright
 * notice and this notice are preserved.
 */

#include "textchan-group.h"

#include <telepathy-glib/base-connection.h>
#include <telepathy-glib/channel-iface.h>
#include <telepathy-glib/dbus.h>
#include <telepathy-glib/dbus-properties-mixin.h>
#include <telepathy-glib/interfaces.h>
#include <telepathy-glib/svc-channel.h>
#include <telepathy-glib/svc-generic.h>

static void text_iface_init (gpointer iface, gpointer data);

G_DEFINE_TYPE_WITH_CODE (TpTestsTextChannelGroup,
    tp_tests_text_channel_group, TP_TYPE_BASE_CHANNEL,
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_CHANNEL_TYPE_TEXT, text_iface_init);
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_CHANNEL_INTERFACE_GROUP,
      tp_group_mixin_iface_init);
    G_IMPLEMENT_INTERFACE (TP_TYPE_SVC_DBUS_PROPERTIES,
      tp_dbus_properties_mixin_iface_init))

static const char *text_channel_group_interfaces[] = {
    TP_IFACE_CHANNEL_INTERFACE_GROUP,
    NULL
};

/* type definition stuff */

enum
{
  PROP_DETAILED = 1,
  PROP_PROPERTIES,
  N_PROPS
};

struct _TpTestsTextChannelGroupPrivate
{
  gboolean detailed;
  gboolean properties;

  gboolean closed;
  gboolean disposed;
};


static gboolean
add_member (GObject *obj,
            TpHandle handle,
            const gchar *message,
            GError **error)
{
  TpTestsTextChannelGroup *self = TP_TESTS_TEXT_CHANNEL_GROUP (obj);
  TpIntset *add = tp_intset_new ();

  tp_intset_add (add, handle);
  tp_group_mixin_change_members (obj, message, add, NULL, NULL, NULL,
      self->conn->self_handle, TP_CHANNEL_GROUP_CHANGE_REASON_NONE);
  tp_intset_destroy (add);

  return TRUE;
}

static gboolean
remove_with_reason (GObject *obj,
    TpHandle handle,
    const gchar *message,
    guint reason,
    GError **error)
{
  TpTestsTextChannelGroup *self = TP_TESTS_TEXT_CHANNEL_GROUP (obj);
  TpGroupMixin *group = TP_GROUP_MIXIN (self);

  tp_clear_pointer (&self->removed_message, g_free);

  self->removed_handle = handle;
  self->removed_message = g_strdup (message);
  self->removed_reason = reason;

  if (handle == group->self_handle)
    {
      /* User wants to leave */
      if (!self->priv->closed)
        {
          self->priv->closed = TRUE;
          tp_svc_channel_emit_closed (self);
        }

      return TRUE;
    }

  return TRUE;
}

static void
tp_tests_text_channel_group_init (TpTestsTextChannelGroup *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
      TP_TESTS_TYPE_TEXT_CHANNEL_GROUP, TpTestsTextChannelGroupPrivate);
}

static GObject *
constructor (GType type,
             guint n_props,
             GObjectConstructParam *props)
{
  GObject *object =
      G_OBJECT_CLASS (tp_tests_text_channel_group_parent_class)->constructor (type,
          n_props, props);
  TpTestsTextChannelGroup *self = TP_TESTS_TEXT_CHANNEL_GROUP (object);
  TpHandleRepoIface *contact_repo;
  TpChannelGroupFlags flags = 0;
  TpBaseChannel *base = TP_BASE_CHANNEL (self);

  self->conn = tp_base_channel_get_connection (base);

  contact_repo = tp_base_connection_get_handles (self->conn,
      TP_HANDLE_TYPE_CONTACT);

  tp_base_channel_register (base);

  tp_text_mixin_init (object, G_STRUCT_OFFSET (TpTestsTextChannelGroup, text),
      contact_repo);

  tp_text_mixin_set_message_types (object,
      TP_CHANNEL_TEXT_MESSAGE_TYPE_NORMAL,
      TP_CHANNEL_TEXT_MESSAGE_TYPE_ACTION,
      TP_CHANNEL_TEXT_MESSAGE_TYPE_NOTICE,
      G_MAXUINT);

  if (self->priv->detailed)
    flags |= TP_CHANNEL_GROUP_FLAG_MEMBERS_CHANGED_DETAILED;

  if (self->priv->properties)
    flags |= TP_CHANNEL_GROUP_FLAG_PROPERTIES;

  tp_group_mixin_init (object, G_STRUCT_OFFSET (TpTestsTextChannelGroup, group),
      contact_repo, self->conn->self_handle);
  tp_group_mixin_change_flags (object, flags, 0);

  return object;
}

static void
get_property (GObject *object,
              guint property_id,
              GValue *value,
              GParamSpec *pspec)
{
  TpTestsTextChannelGroup *self = TP_TESTS_TEXT_CHANNEL_GROUP (object);

  switch (property_id)
    {
    case PROP_DETAILED:
      g_value_set_boolean (value, self->priv->detailed);
      break;
    case PROP_PROPERTIES:
      g_value_set_boolean (value, self->priv->properties);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
set_property (GObject *object,
              guint property_id,
              const GValue *value,
              GParamSpec *pspec)
{
  TpTestsTextChannelGroup *self = TP_TESTS_TEXT_CHANNEL_GROUP (object);

  switch (property_id)
    {
    case PROP_DETAILED:
      self->priv->detailed = g_value_get_boolean (value);
      break;
    case PROP_PROPERTIES:
      self->priv->properties = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
dispose (GObject *object)
{
  TpTestsTextChannelGroup *self = TP_TESTS_TEXT_CHANNEL_GROUP (object);

  if (self->priv->disposed)
    return;

  self->priv->disposed = TRUE;

  if (!self->priv->closed)
    {
      tp_svc_channel_emit_closed (self);
    }

  ((GObjectClass *) tp_tests_text_channel_group_parent_class)->dispose (object);
}

static void
finalize (GObject *object)
{
  tp_text_mixin_finalize (object);
  tp_group_mixin_finalize (object);

  ((GObjectClass *) tp_tests_text_channel_group_parent_class)->finalize (object);
}

static void
channel_close (TpBaseChannel *base)
{
  TpTestsTextChannelGroup *self = TP_TESTS_TEXT_CHANNEL_GROUP (base);

  if (!self->priv->closed)
    {
      self->priv->closed = TRUE;
      tp_svc_channel_emit_closed (self);
    }
}

static void
tp_tests_text_channel_group_class_init (TpTestsTextChannelGroupClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;
  GParamSpec *param_spec;
  TpBaseChannelClass *base_class = (TpBaseChannelClass *) klass;

  g_type_class_add_private (klass, sizeof (TpTestsTextChannelGroupPrivate));

  object_class->constructor = constructor;
  object_class->set_property = set_property;
  object_class->get_property = get_property;
  object_class->dispose = dispose;
  object_class->finalize = finalize;

  base_class->channel_type = TP_IFACE_CHANNEL_TYPE_TEXT;
  base_class->target_handle_type = TP_HANDLE_TYPE_NONE;
  base_class->interfaces = text_channel_group_interfaces;
  base_class->close = channel_close;

  param_spec = g_param_spec_boolean ("detailed",
      "Has the Members_Changed_Detailed flag?",
      "True if the Members_Changed_Detailed group flag should be set",
      TRUE,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_DETAILED, param_spec);

  param_spec = g_param_spec_boolean ("properties",
      "Has the Properties flag?",
      "True if the Properties group flag should be set",
      TRUE,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_PROPERTIES, param_spec);

  tp_text_mixin_class_init (object_class,
      G_STRUCT_OFFSET (TpTestsTextChannelGroupClass, text_class));
  tp_group_mixin_class_init (object_class,
      G_STRUCT_OFFSET (TpTestsTextChannelGroupClass, group_class), add_member,
      NULL);

  tp_group_mixin_class_set_remove_with_reason_func (object_class,
      remove_with_reason);

  tp_group_mixin_class_allow_self_removal (object_class);

  tp_dbus_properties_mixin_class_init (object_class,
      G_STRUCT_OFFSET (TpTestsTextChannelGroupClass, dbus_properties_class));

  tp_group_mixin_init_dbus_properties (object_class);
}

static void
text_send (TpSvcChannelTypeText *iface,
           guint type,
           const gchar *text,
           DBusGMethodInvocation *context)
{
  /* silently swallow the message */
  tp_svc_channel_type_text_return_from_send (context);
}

static void
text_iface_init (gpointer iface,
                 gpointer data)
{
  TpSvcChannelTypeTextClass *klass = iface;

  tp_text_mixin_iface_init (iface, data);
#define IMPLEMENT(x) tp_svc_channel_type_text_implement_##x (klass, text_##x)
  IMPLEMENT (send);
#undef IMPLEMENT
}

void
tp_tests_text_channel_group_join (TpTestsTextChannelGroup *self)
{
  TpIntset *add, *empty;

 /* Add ourself as a member */
  add = tp_intset_new_containing (self->conn->self_handle);
  empty = tp_intset_new ();

  tp_group_mixin_change_members ((GObject *) self, NULL, add, empty,
      empty, empty, 0, 0);

  tp_intset_destroy (add);
  tp_intset_destroy (empty);
}
