/*
 * im-manager.c - an example channel manager for channels talking to a
 * particular contact. Similar code is used for 1-1 IM channels in many
 * protocols (IRC private messages ("/query"), XMPP IM etc.)
 *
 * Copyright (C) 2007 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2007 Nokia Corporation
 *
 * Copying and distribution of this file, with or without modification,
 * are permitted in any medium without royalty provided the copyright
 * notice and this notice are preserved.
 */

#include "im-manager.h"

#include <dbus/dbus-glib.h>

#include <telepathy-glib/base-connection.h>
#include <telepathy-glib/channel-manager.h>
#include <telepathy-glib/dbus.h>
#include <telepathy-glib/errors.h>
#include <telepathy-glib/interfaces.h>

#include "chan.h"

static void channel_manager_iface_init (gpointer, gpointer);

G_DEFINE_TYPE_WITH_CODE (ExampleEchoImManager,
    example_echo_im_manager,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE (TP_TYPE_CHANNEL_MANAGER,
      channel_manager_iface_init))

/* type definition stuff */

enum
{
  PROP_CONNECTION = 1,
  N_PROPS
};

struct _ExampleEchoImManagerPrivate
{
  TpBaseConnection *conn;

  /* GUINT_TO_POINTER (handle) => ExampleEchoChannel */
  GHashTable *channels;
  gulong status_changed_id;
};

static void
example_echo_im_manager_init (ExampleEchoImManager *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, EXAMPLE_TYPE_ECHO_IM_MANAGER,
      ExampleEchoImManagerPrivate);

  self->priv->channels = g_hash_table_new_full (g_direct_hash, g_direct_equal,
      NULL, g_object_unref);
}

static void example_echo_im_manager_close_all (ExampleEchoImManager *self);

static void
dispose (GObject *object)
{
  ExampleEchoImManager *self = EXAMPLE_ECHO_IM_MANAGER (object);

  example_echo_im_manager_close_all (self);
  g_assert (self->priv->channels == NULL);

  ((GObjectClass *) example_echo_im_manager_parent_class)->dispose (object);
}

static void
get_property (GObject *object,
              guint property_id,
              GValue *value,
              GParamSpec *pspec)
{
  ExampleEchoImManager *self = EXAMPLE_ECHO_IM_MANAGER (object);

  switch (property_id)
    {
    case PROP_CONNECTION:
      g_value_set_object (value, self->priv->conn);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
set_property (GObject *object,
              guint property_id,
              const GValue *value,
              GParamSpec *pspec)
{
  ExampleEchoImManager *self = EXAMPLE_ECHO_IM_MANAGER (object);

  switch (property_id)
    {
    case PROP_CONNECTION:
      /* We don't ref the connection, because it owns a reference to the
       * channel manager, and it guarantees that the manager's lifetime is
       * less than its lifetime */
      self->priv->conn = g_value_get_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
status_changed_cb (TpBaseConnection *conn,
                   guint status,
                   guint reason,
                   ExampleEchoImManager *self)
{
  if (status == TP_CONNECTION_STATUS_DISCONNECTED)
    example_echo_im_manager_close_all (self);
}

static void
constructed (GObject *object)
{
  ExampleEchoImManager *self = EXAMPLE_ECHO_IM_MANAGER (object);
  void (*chain_up) (GObject *) =
      ((GObjectClass *) example_echo_im_manager_parent_class)->constructed;

  if (chain_up != NULL)
    {
      chain_up (object);
    }

  self->priv->status_changed_id = g_signal_connect (self->priv->conn,
      "status-changed", (GCallback) status_changed_cb, self);
}

static void
example_echo_im_manager_class_init (ExampleEchoImManagerClass *klass)
{
  GParamSpec *param_spec;
  GObjectClass *object_class = (GObjectClass *) klass;

  object_class->constructed = constructed;
  object_class->dispose = dispose;
  object_class->get_property = get_property;
  object_class->set_property = set_property;

  param_spec = g_param_spec_object ("connection", "Connection object",
      "The connection that owns this channel manager",
      TP_TYPE_BASE_CONNECTION,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE |
      G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB);
  g_object_class_install_property (object_class, PROP_CONNECTION, param_spec);

  g_type_class_add_private (klass, sizeof (ExampleEchoImManagerPrivate));
}

static void
example_echo_im_manager_close_all (ExampleEchoImManager *self)
{
  if (self->priv->channels != NULL)
    {
      GHashTable *tmp = self->priv->channels;

      self->priv->channels = NULL;
      g_hash_table_destroy (tmp);
    }

  if (self->priv->status_changed_id != 0)
    {
      g_signal_handler_disconnect (self->priv->conn,
          self->priv->status_changed_id);
      self->priv->status_changed_id = 0;
    }
}

static void
example_echo_im_manager_foreach_channel (TpChannelManager *iface,
                                         TpExportableChannelFunc callback,
                                         gpointer user_data)
{
  ExampleEchoImManager *self = EXAMPLE_ECHO_IM_MANAGER (iface);
  GHashTableIter iter;
  gpointer handle, channel;

  g_hash_table_iter_init (&iter, self->priv->channels);

  while (g_hash_table_iter_next (&iter, &handle, &channel))
    {
      callback (TP_EXPORTABLE_CHANNEL (channel), user_data);
    }
}

static void
channel_closed_cb (ExampleEchoChannel *chan,
                   ExampleEchoImManager *self)
{
  tp_channel_manager_emit_channel_closed_for_object (self,
      TP_EXPORTABLE_CHANNEL (chan));

  if (self->priv->channels != NULL)
    {
      TpHandle handle;
      gboolean really_destroyed;

      g_object_get (chan,
          "handle", &handle,
          "channel-destroyed", &really_destroyed,
          NULL);

      /* Re-announce the channel if it's not yet ready to go away (pending
       * messages) */
      if (really_destroyed)
        {
          g_hash_table_remove (self->priv->channels,
              GUINT_TO_POINTER (handle));
        }
      else
        {
          tp_channel_manager_emit_new_channel (self,
              TP_EXPORTABLE_CHANNEL (chan), NULL);
        }
    }
}

static ExampleEchoChannel *
new_channel (ExampleEchoImManager *self,
             TpHandle handle,
             TpHandle initiator,
             gpointer request_token)
{
  ExampleEchoChannel *chan;
  gchar *object_path;
  GSList *requests = NULL;

  object_path = g_strdup_printf ("%s/EchoChannel%u",
      self->priv->conn->object_path, handle);

  chan = g_object_new (EXAMPLE_TYPE_ECHO_CHANNEL,
      "connection", self->priv->conn,
      "object-path", object_path,
      "handle", handle,
      "initiator-handle", initiator,
      NULL);

  g_free (object_path);

  g_signal_connect (chan, "closed", (GCallback) channel_closed_cb, self);

  g_hash_table_insert (self->priv->channels, GUINT_TO_POINTER (handle), chan);

  if (request_token != NULL)
    requests = g_slist_prepend (requests, request_token);

  tp_channel_manager_emit_new_channel (self, TP_EXPORTABLE_CHANNEL (chan),
      requests);
  g_slist_free (requests);

  return chan;
}

static const gchar * const fixed_properties[] = {
    TP_IFACE_CHANNEL ".ChannelType",
    TP_IFACE_CHANNEL ".TargetHandleType",
    NULL
};

static const gchar * const allowed_properties[] = {
    TP_IFACE_CHANNEL ".TargetHandle",
    TP_IFACE_CHANNEL ".TargetID",
    NULL
};

static void
example_echo_im_manager_foreach_channel_class (TpChannelManager *manager,
    TpChannelManagerChannelClassFunc func,
    gpointer user_data)
{
  GHashTable *table = g_hash_table_new_full (g_str_hash, g_str_equal,
      NULL, (GDestroyNotify) tp_g_value_slice_free);
  GValue *value;

  value = tp_g_value_slice_new (G_TYPE_STRING);
  g_value_set_static_string (value, TP_IFACE_CHANNEL_TYPE_TEXT);
  g_hash_table_insert (table, TP_IFACE_CHANNEL ".ChannelType", value);

  value = tp_g_value_slice_new (G_TYPE_UINT);
  g_value_set_uint (value, TP_HANDLE_TYPE_CONTACT);
  g_hash_table_insert (table, TP_IFACE_CHANNEL ".TargetHandleType", value);

  func (manager, table, allowed_properties, user_data);

  g_hash_table_destroy (table);
}

static gboolean
example_echo_im_manager_request (ExampleEchoImManager *self,
                                 gpointer request_token,
                                 GHashTable *request_properties,
                                 gboolean require_new)
{
  TpHandle handle;
  ExampleEchoChannel *chan;
  GError *error = NULL;

  if (tp_strdiff (tp_asv_get_string (request_properties,
          TP_IFACE_CHANNEL ".ChannelType"),
      TP_IFACE_CHANNEL_TYPE_TEXT))
    {
      return FALSE;
    }

  if (tp_asv_get_uint32 (request_properties,
      TP_IFACE_CHANNEL ".TargetHandleType", NULL) != TP_HANDLE_TYPE_CONTACT)
    {
      return FALSE;
    }

  handle = tp_asv_get_uint32 (request_properties,
      TP_IFACE_CHANNEL ".TargetHandle", NULL);
  g_assert (handle != 0);

  if (tp_channel_manager_asv_has_unknown_properties (request_properties,
        fixed_properties, allowed_properties, &error))
    {
      goto error;
    }

  chan = g_hash_table_lookup (self->priv->channels, GUINT_TO_POINTER (handle));

  if (chan == NULL)
    {
      chan = new_channel (self, handle, self->priv->conn->self_handle,
          request_token);
    }
  else if (require_new)
    {
      g_set_error (&error, TP_ERRORS, TP_ERROR_NOT_AVAILABLE,
          "An echo channel to contact #%u already exists", handle);
      goto error;
    }
  else
    {
      tp_channel_manager_emit_request_already_satisfied (self,
          request_token, TP_EXPORTABLE_CHANNEL (chan));
    }

  return TRUE;

error:
  tp_channel_manager_emit_request_failed (self, request_token,
      error->domain, error->code, error->message);
  g_error_free (error);
  return TRUE;
}

static gboolean
example_echo_im_manager_create_channel (TpChannelManager *manager,
                                        gpointer request_token,
                                        GHashTable *request_properties)
{
    return example_echo_im_manager_request (EXAMPLE_ECHO_IM_MANAGER (manager),
        request_token, request_properties, TRUE);
}

static gboolean
example_echo_im_manager_ensure_channel (TpChannelManager *manager,
                                        gpointer request_token,
                                        GHashTable *request_properties)
{
    return example_echo_im_manager_request (EXAMPLE_ECHO_IM_MANAGER (manager),
        request_token, request_properties, FALSE);
}

static void
channel_manager_iface_init (gpointer g_iface,
                            gpointer iface_data G_GNUC_UNUSED)
{
  TpChannelManagerIface *iface = g_iface;

  iface->foreach_channel = example_echo_im_manager_foreach_channel;
  iface->foreach_channel_class = example_echo_im_manager_foreach_channel_class;
  iface->create_channel = example_echo_im_manager_create_channel;
  iface->ensure_channel = example_echo_im_manager_ensure_channel;
  /* In this channel manager, Request has the same semantics as Ensure */
  iface->request_channel = example_echo_im_manager_ensure_channel;
}
