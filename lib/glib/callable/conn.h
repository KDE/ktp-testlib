/*
 * conn.h - header for an example connection
 *
 * Copyright © 2007-2009 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright © 2007-2009 Nokia Corporation
 *
 * Copying and distribution of this file, with or without modification,
 * are permitted in any medium without royalty provided the copyright
 * notice and this notice are preserved.
 */

#ifndef __EXAMPLE_CALLABLE_CONN_H__
#define __EXAMPLE_CALLABLE_CONN_H__

#include <glib-object.h>
#include <telepathy-glib/base-connection.h>
#include <telepathy-glib/contacts-mixin.h>
#include <telepathy-glib/presence-mixin.h>

G_BEGIN_DECLS

typedef struct _ExampleCallableConnection ExampleCallableConnection;
typedef struct _ExampleCallableConnectionPrivate
    ExampleCallableConnectionPrivate;

typedef struct _ExampleCallableConnectionClass ExampleCallableConnectionClass;
typedef struct _ExampleCallableConnectionClassPrivate
    ExampleCallableConnectionClassPrivate;

struct _ExampleCallableConnectionClass {
    TpBaseConnectionClass parent_class;
    TpPresenceMixinClass presence_mixin;
    TpContactsMixinClass contacts_mixin;

    ExampleCallableConnectionClassPrivate *priv;
};

struct _ExampleCallableConnection {
    TpBaseConnection parent;
    TpPresenceMixin presence_mixin;
    TpContactsMixin contacts_mixin;

    ExampleCallableConnectionPrivate *priv;
};

GType example_callable_connection_get_type (void);

#define EXAMPLE_TYPE_CALLABLE_CONNECTION \
  (example_callable_connection_get_type ())
#define EXAMPLE_CALLABLE_CONNECTION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), EXAMPLE_TYPE_CALLABLE_CONNECTION, \
                              ExampleCallableConnection))
#define EXAMPLE_CALLABLE_CONNECTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), EXAMPLE_TYPE_CALLABLE_CONNECTION, \
                           ExampleCallableConnectionClass))
#define EXAMPLE_IS_CALLABLE_CONNECTION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), EXAMPLE_TYPE_CALLABLE_CONNECTION))
#define EXAMPLE_IS_CALLABLE_CONNECTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass), EXAMPLE_TYPE_CALLABLE_CONNECTION))
#define EXAMPLE_CALLABLE_CONNECTION_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXAMPLE_TYPE_CALLABLE_CONNECTION, \
                              ExampleCallableConnectionClass))

gchar *example_callable_normalize_contact (TpHandleRepoIface *repo,
    const gchar *id, gpointer context, GError **error);

/* Must be kept in sync with the array presence_statuses in conn.c */
typedef enum {
    EXAMPLE_CALLABLE_PRESENCE_OFFLINE = 0,
    EXAMPLE_CALLABLE_PRESENCE_UNKNOWN,
    EXAMPLE_CALLABLE_PRESENCE_ERROR,
    EXAMPLE_CALLABLE_PRESENCE_AWAY,
    EXAMPLE_CALLABLE_PRESENCE_AVAILABLE
} ExampleCallablePresence;

G_END_DECLS

#endif
