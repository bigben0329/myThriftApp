/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include <string.h>
#include <thrift/c_glib/thrift.h>
#include <thrift/c_glib/thrift_application_exception.h>
#include "bulletin_board.h"

gboolean
bulletin_board_if_add (BulletinBoardIf *iface, const Message * msg, GError **error)
{
  return BULLETIN_BOARD_IF_GET_INTERFACE (iface)->add (iface, msg, error);
}

gboolean
bulletin_board_if_get (BulletinBoardIf *iface, GPtrArray ** _return, GError **error)
{
  return BULLETIN_BOARD_IF_GET_INTERFACE (iface)->get (iface, _return, error);
}

GType
bulletin_board_if_get_type (void)
{
  static GType type = 0;
  if (type == 0)
  {
    static const GTypeInfo type_info =
    {
      sizeof (BulletinBoardIfInterface),
      NULL,  /* base_init */
      NULL,  /* base_finalize */
      NULL,  /* class_init */
      NULL,  /* class_finalize */
      NULL,  /* class_data */
      0,     /* instance_size */
      0,     /* n_preallocs */
      NULL,  /* instance_init */
      NULL   /* value_table */
    };
    type = g_type_register_static (G_TYPE_INTERFACE,
                                   "BulletinBoardIf",
                                   &type_info, 0);
  }
  return type;
}

static void 
bulletin_board_if_interface_init (BulletinBoardIfInterface *iface);

G_DEFINE_TYPE_WITH_CODE (BulletinBoardClient, bulletin_board_client,
                       G_TYPE_OBJECT, 
                         G_IMPLEMENT_INTERFACE (TYPE_BULLETIN_BOARD_IF,
                                                bulletin_board_if_interface_init));

enum _BulletinBoardClientProperties
{
  PROP_0,
  PROP_BULLETIN_BOARD_CLIENT_INPUT_PROTOCOL,
  PROP_BULLETIN_BOARD_CLIENT_OUTPUT_PROTOCOL
};

void
bulletin_board_client_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
  BulletinBoardClient *client = BULLETIN_BOARD_CLIENT (object);

  THRIFT_UNUSED_VAR (pspec);

  switch (property_id)
  {
    case PROP_BULLETIN_BOARD_CLIENT_INPUT_PROTOCOL:
      client->input_protocol = g_value_get_object (value);
      break;
    case PROP_BULLETIN_BOARD_CLIENT_OUTPUT_PROTOCOL:
      client->output_protocol = g_value_get_object (value);
      break;
  }
}

void
bulletin_board_client_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
  BulletinBoardClient *client = BULLETIN_BOARD_CLIENT (object);

  THRIFT_UNUSED_VAR (pspec);

  switch (property_id)
  {
    case PROP_BULLETIN_BOARD_CLIENT_INPUT_PROTOCOL:
      g_value_set_object (value, client->input_protocol);
      break;
    case PROP_BULLETIN_BOARD_CLIENT_OUTPUT_PROTOCOL:
      g_value_set_object (value, client->output_protocol);
      break;
  }
}

gboolean bulletin_board_client_send_add (BulletinBoardIf * iface, const Message * msg, GError ** error)
{
  gint32 cseqid = 0;
  ThriftProtocol * protocol = BULLETIN_BOARD_CLIENT (iface)->output_protocol;

  if (thrift_protocol_write_message_begin (protocol, "add", T_CALL, cseqid, error) < 0)
    return FALSE;

  {
    gint32 ret;
    gint32 xfer = 0;

    
    if ((ret = thrift_protocol_write_struct_begin (protocol, "add_args", error)) < 0)
      return 0;
    xfer += ret;
    if ((ret = thrift_protocol_write_field_begin (protocol, "msg", T_STRUCT, 1, error)) < 0)
      return 0;
    xfer += ret;
    if ((ret = thrift_struct_write (THRIFT_STRUCT (msg), protocol, error)) < 0)
      return 0;
    xfer += ret;

    if ((ret = thrift_protocol_write_field_end (protocol, error)) < 0)
      return 0;
    xfer += ret;
    if ((ret = thrift_protocol_write_field_stop (protocol, error)) < 0)
      return 0;
    xfer += ret;
    if ((ret = thrift_protocol_write_struct_end (protocol, error)) < 0)
      return 0;
    xfer += ret;

  }

  if (thrift_protocol_write_message_end (protocol, error) < 0)
    return FALSE;
  if (!thrift_transport_flush (protocol->transport, error))
    return FALSE;
  if (!thrift_transport_write_end (protocol->transport, error))
    return FALSE;

  return TRUE;
}

gboolean bulletin_board_client_recv_add (BulletinBoardIf * iface, GError ** error)
{

  gint32 rseqid;
  gchar * fname;
  ThriftMessageType mtype;
  ThriftProtocol * protocol = BULLETIN_BOARD_CLIENT (iface)->input_protocol;

  if (thrift_protocol_read_message_begin (protocol, &fname, &mtype, &rseqid, error) < 0)
  {
    if (fname) g_free (fname);
    return FALSE;
  }

  if (mtype == T_EXCEPTION) {
    if (fname) g_free (fname);
    ThriftApplicationException *xception = g_object_new (THRIFT_TYPE_APPLICATION_EXCEPTION, NULL);
    thrift_struct_read (THRIFT_STRUCT (xception), protocol, NULL);
    thrift_protocol_read_message_end (protocol, NULL);
    thrift_transport_read_end (protocol->transport, NULL);
    g_set_error (error, THRIFT_APPLICATION_EXCEPTION_ERROR, xception->type, "application error: %s", xception->message);
    g_object_unref (xception);
    return FALSE;
  } else if (mtype != T_REPLY) {
    if (fname) g_free (fname);
    thrift_protocol_skip (protocol, T_STRUCT, NULL);
    thrift_protocol_read_message_end (protocol, NULL);
    thrift_transport_read_end (protocol->transport, NULL);
    g_set_error (error, THRIFT_APPLICATION_EXCEPTION_ERROR, THRIFT_APPLICATION_EXCEPTION_ERROR_INVALID_MESSAGE_TYPE, "invalid message type %d, expected T_REPLY", mtype);
    return FALSE;
  } else if (strncmp (fname, "add", 3) != 0) {
    thrift_protocol_skip (protocol, T_STRUCT, NULL);
    thrift_protocol_read_message_end (protocol, error);
    thrift_transport_read_end (protocol->transport, error);
    g_set_error (error, THRIFT_APPLICATION_EXCEPTION_ERROR, THRIFT_APPLICATION_EXCEPTION_ERROR_WRONG_METHOD_NAME, "wrong method name %s, expected add", fname);
    if (fname) g_free (fname);
    return FALSE;
  }
  if (fname) g_free (fname);

  {
    gint32 ret;
    gint32 xfer = 0;
    gchar *name = NULL;
    ThriftType ftype;
    gint16 fid;
    guint32 len = 0;
    gpointer data = NULL;
    

    /* satisfy -Wall in case these aren't used */
    THRIFT_UNUSED_VAR (len);
    THRIFT_UNUSED_VAR (data);

    /* read the struct begin marker */
    if ((ret = thrift_protocol_read_struct_begin (protocol, &name, error)) < 0)
    {
      if (name) g_free (name);
      return 0;
    }
    xfer += ret;
    if (name) g_free (name);
    name = NULL;

    /* read the struct fields */
    while (1)
    {
      /* read the beginning of a field */
      if ((ret = thrift_protocol_read_field_begin (protocol, &name, &ftype, &fid, error)) < 0)
      {
        if (name) g_free (name);
        return 0;
      }
      xfer += ret;
      if (name) g_free (name);
      name = NULL;

      /* break if we get a STOP field */
      if (ftype == T_STOP)
      {
        break;
      }

      switch (fid)
      {
        default:
          if ((ret = thrift_protocol_skip (protocol, ftype, error)) < 0)
            return 0;
          xfer += ret;
          break;
      }
      if ((ret = thrift_protocol_read_field_end (protocol, error)) < 0)
        return 0;
      xfer += ret;
    }

    if ((ret = thrift_protocol_read_struct_end (protocol, error)) < 0)
      return 0;
    xfer += ret;

  }

  if (thrift_protocol_read_message_end (protocol, error) < 0)
    return FALSE;

  if (!thrift_transport_read_end (protocol->transport, error))
    return FALSE;

  return TRUE;
}

gboolean bulletin_board_client_add (BulletinBoardIf * iface, const Message * msg, GError ** error)
{
  if (!bulletin_board_client_send_add (iface, msg, error))
    return FALSE;
  if (!bulletin_board_client_recv_add (iface, error))
    return FALSE;
  return TRUE;
}

gboolean bulletin_board_client_send_get (BulletinBoardIf * iface, GError ** error)
{
  gint32 cseqid = 0;
  ThriftProtocol * protocol = BULLETIN_BOARD_CLIENT (iface)->output_protocol;

  if (thrift_protocol_write_message_begin (protocol, "get", T_CALL, cseqid, error) < 0)
    return FALSE;

  {
    gint32 ret;
    gint32 xfer = 0;

    
    if ((ret = thrift_protocol_write_struct_begin (protocol, "get_args", error)) < 0)
      return 0;
    xfer += ret;
    if ((ret = thrift_protocol_write_field_stop (protocol, error)) < 0)
      return 0;
    xfer += ret;
    if ((ret = thrift_protocol_write_struct_end (protocol, error)) < 0)
      return 0;
    xfer += ret;

  }

  if (thrift_protocol_write_message_end (protocol, error) < 0)
    return FALSE;
  if (!thrift_transport_flush (protocol->transport, error))
    return FALSE;
  if (!thrift_transport_write_end (protocol->transport, error))
    return FALSE;

  return TRUE;
}

gboolean bulletin_board_client_recv_get (BulletinBoardIf * iface, GPtrArray ** _return, GError ** error)
{

  gint32 rseqid;
  gchar * fname;
  ThriftMessageType mtype;
  ThriftProtocol * protocol = BULLETIN_BOARD_CLIENT (iface)->input_protocol;

  if (thrift_protocol_read_message_begin (protocol, &fname, &mtype, &rseqid, error) < 0)
  {
    if (fname) g_free (fname);
    return FALSE;
  }

  if (mtype == T_EXCEPTION) {
    if (fname) g_free (fname);
    ThriftApplicationException *xception = g_object_new (THRIFT_TYPE_APPLICATION_EXCEPTION, NULL);
    thrift_struct_read (THRIFT_STRUCT (xception), protocol, NULL);
    thrift_protocol_read_message_end (protocol, NULL);
    thrift_transport_read_end (protocol->transport, NULL);
    g_set_error (error, THRIFT_APPLICATION_EXCEPTION_ERROR, xception->type, "application error: %s", xception->message);
    g_object_unref (xception);
    return FALSE;
  } else if (mtype != T_REPLY) {
    if (fname) g_free (fname);
    thrift_protocol_skip (protocol, T_STRUCT, NULL);
    thrift_protocol_read_message_end (protocol, NULL);
    thrift_transport_read_end (protocol->transport, NULL);
    g_set_error (error, THRIFT_APPLICATION_EXCEPTION_ERROR, THRIFT_APPLICATION_EXCEPTION_ERROR_INVALID_MESSAGE_TYPE, "invalid message type %d, expected T_REPLY", mtype);
    return FALSE;
  } else if (strncmp (fname, "get", 3) != 0) {
    thrift_protocol_skip (protocol, T_STRUCT, NULL);
    thrift_protocol_read_message_end (protocol, error);
    thrift_transport_read_end (protocol->transport, error);
    g_set_error (error, THRIFT_APPLICATION_EXCEPTION_ERROR, THRIFT_APPLICATION_EXCEPTION_ERROR_WRONG_METHOD_NAME, "wrong method name %s, expected get", fname);
    if (fname) g_free (fname);
    return FALSE;
  }
  if (fname) g_free (fname);

  {
    gint32 ret;
    gint32 xfer = 0;
    gchar *name = NULL;
    ThriftType ftype;
    gint16 fid;
    guint32 len = 0;
    gpointer data = NULL;
    

    /* satisfy -Wall in case these aren't used */
    THRIFT_UNUSED_VAR (len);
    THRIFT_UNUSED_VAR (data);

    /* read the struct begin marker */
    if ((ret = thrift_protocol_read_struct_begin (protocol, &name, error)) < 0)
    {
      if (name) g_free (name);
      return 0;
    }
    xfer += ret;
    if (name) g_free (name);
    name = NULL;

    /* read the struct fields */
    while (1)
    {
      /* read the beginning of a field */
      if ((ret = thrift_protocol_read_field_begin (protocol, &name, &ftype, &fid, error)) < 0)
      {
        if (name) g_free (name);
        return 0;
      }
      xfer += ret;
      if (name) g_free (name);
      name = NULL;

      /* break if we get a STOP field */
      if (ftype == T_STOP)
      {
        break;
      }

      switch (fid)
      {
        case 0:
          if (ftype == T_LIST)
          {
            {
              guint32 size;
              ThriftType element_type;
              if ((ret = thrift_protocol_read_list_begin (protocol, &element_type, &size, error)) < 0)
                return 0;
              xfer += ret;

              /* iterate through list elements */
              guint32 i;
              for (i = 0; i < size; i++)
              {
                Message * _elem0 = NULL;
                if ( _elem0 != NULL)
                {
                  g_object_unref (_elem0);
                }
                _elem0 = g_object_new (TYPE_MESSAGE, NULL);
                if ((ret = thrift_struct_read (THRIFT_STRUCT (_elem0), protocol, error)) < 0)
                {
                  g_object_unref (_elem0);
                  return 0;
                }
                xfer += ret;
                g_ptr_array_add (*_return, _elem0);
              }
              if ((ret = thrift_protocol_read_list_end (protocol, error)) < 0)
                return 0;
              xfer += ret;

            }
          } else {
            if ((ret = thrift_protocol_skip (protocol, ftype, error)) < 0)
              return 0;
            xfer += ret;
          }
          break;
        default:
          if ((ret = thrift_protocol_skip (protocol, ftype, error)) < 0)
            return 0;
          xfer += ret;
          break;
      }
      if ((ret = thrift_protocol_read_field_end (protocol, error)) < 0)
        return 0;
      xfer += ret;
    }

    if ((ret = thrift_protocol_read_struct_end (protocol, error)) < 0)
      return 0;
    xfer += ret;

  }

  if (thrift_protocol_read_message_end (protocol, error) < 0)
    return FALSE;

  if (!thrift_transport_read_end (protocol->transport, error))
    return FALSE;

  return TRUE;
}

gboolean bulletin_board_client_get (BulletinBoardIf * iface, GPtrArray ** _return, GError ** error)
{
  if (!bulletin_board_client_send_get (iface, error))
    return FALSE;
  if (!bulletin_board_client_recv_get (iface, _return, error))
    return FALSE;
  return TRUE;
}

static void
bulletin_board_if_interface_init (BulletinBoardIfInterface *iface)
{
  iface->add = bulletin_board_client_add;
  iface->get = bulletin_board_client_get;
}

static void
bulletin_board_client_init (BulletinBoardClient *client)
{
  client->input_protocol = NULL;
  client->output_protocol = NULL;
}

static void
bulletin_board_client_class_init (BulletinBoardClientClass *cls)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (cls);
  GParamSpec *param_spec;

  gobject_class->set_property = bulletin_board_client_set_property;
  gobject_class->get_property = bulletin_board_client_get_property;

  param_spec = g_param_spec_object ("input_protocol",
                                    "input protocol (construct)",
                                    "Set the client input protocol",
                                    THRIFT_TYPE_PROTOCOL,
                                    G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_BULLETIN_BOARD_CLIENT_INPUT_PROTOCOL, param_spec);

  param_spec = g_param_spec_object ("output_protocol",
                                    "output protocol (construct)",
                                    "Set the client output protocol",
                                    THRIFT_TYPE_PROTOCOL,
                                    G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_BULLETIN_BOARD_CLIENT_OUTPUT_PROTOCOL, param_spec);
}
