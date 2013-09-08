/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

#include "idl_types.h"
#include <thrift/c_glib/thrift.h>

/* reads a message object */
static gint32
message_read (ThriftStruct *object, ThriftProtocol *protocol, GError **error)
{
  gint32 ret;
  gint32 xfer = 0;
  gchar *name = NULL;
  ThriftType ftype;
  gint16 fid;
  guint32 len = 0;
  gpointer data = NULL;
  Message * this_object = MESSAGE(object);

  /* satisfy -Wall in case these aren't used */
  THRIFT_UNUSED_VAR (len);
  THRIFT_UNUSED_VAR (data);
  THRIFT_UNUSED_VAR (this_object);

  /* read the struct begin marker */
  if ((ret = thrift_protocol_read_struct_begin (protocol, &name, error)) < 0)
  {
    if (name) g_free (name);
    return -1;
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
      return -1;
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
      case 1:
        if (ftype == T_STRING)
        {
          if (this_object->text != NULL)
          {
            g_free(this_object->text);
            this_object->text = NULL;
          }

          if ((ret = thrift_protocol_read_string (protocol, &this_object->text, error)) < 0)
            return -1;
          xfer += ret;
          this_object->__isset_text = TRUE;
        } else {
          if ((ret = thrift_protocol_skip (protocol, ftype, error)) < 0)
            return -1;
          xfer += ret;
        }
        break;
      case 2:
        if (ftype == T_STRING)
        {
          if (this_object->date != NULL)
          {
            g_free(this_object->date);
            this_object->date = NULL;
          }

          if ((ret = thrift_protocol_read_string (protocol, &this_object->date, error)) < 0)
            return -1;
          xfer += ret;
          this_object->__isset_date = TRUE;
        } else {
          if ((ret = thrift_protocol_skip (protocol, ftype, error)) < 0)
            return -1;
          xfer += ret;
        }
        break;
      default:
        if ((ret = thrift_protocol_skip (protocol, ftype, error)) < 0)
          return -1;
        xfer += ret;
        break;
    }
    if ((ret = thrift_protocol_read_field_end (protocol, error)) < 0)
      return -1;
    xfer += ret;
  }

  if ((ret = thrift_protocol_read_struct_end (protocol, error)) < 0)
    return -1;
  xfer += ret;

  return xfer;
}

static gint32
message_write (ThriftStruct *object, ThriftProtocol *protocol, GError **error)
{
  gint32 ret;
  gint32 xfer = 0;

  Message * this_object = MESSAGE(object);
  THRIFT_UNUSED_VAR (this_object);
  if ((ret = thrift_protocol_write_struct_begin (protocol, "Message", error)) < 0)
    return -1;
  xfer += ret;
  if ((ret = thrift_protocol_write_field_begin (protocol, "text", T_STRING, 1, error)) < 0)
    return -1;
  xfer += ret;
  if ((ret = thrift_protocol_write_string (protocol, this_object->text, error)) < 0)
    return -1;
  if ((ret = thrift_protocol_write_field_end (protocol, error)) < 0)
    return -1;
  xfer += ret;
  if ((ret = thrift_protocol_write_field_begin (protocol, "date", T_STRING, 2, error)) < 0)
    return -1;
  xfer += ret;
  if ((ret = thrift_protocol_write_string (protocol, this_object->date, error)) < 0)
    return -1;
  if ((ret = thrift_protocol_write_field_end (protocol, error)) < 0)
    return -1;
  xfer += ret;
  if ((ret = thrift_protocol_write_field_stop (protocol, error)) < 0)
    return -1;
  xfer += ret;
  if ((ret = thrift_protocol_write_struct_end (protocol, error)) < 0)
    return -1;
  xfer += ret;

  return xfer;
}

static void 
message_instance_init (Message * object)
{
  /* satisfy -Wall */
  THRIFT_UNUSED_VAR (object);
  object->text = NULL;
  object->__isset_text = FALSE;
  object->date = NULL;
  object->__isset_date = FALSE;
}

static void 
message_finalize (GObject *object)
{
  Message *tobject = MESSAGE (object);

  /* satisfy -Wall in case we don't use tobject */
  THRIFT_UNUSED_VAR (tobject);
  if (tobject->text != NULL)
  {
    g_free (tobject->text);
    tobject->text = NULL;
  }
  if (tobject->date != NULL)
  {
    g_free (tobject->date);
    tobject->date = NULL;
  }
}

static void 
message_class_init (ThriftStructClass * cls)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (cls);

  gobject_class->finalize = message_finalize;
  cls->read = message_read;
  cls->write = message_write;
}

GType
message_get_type (void)
{
  static GType type = 0;

  if (type == 0) 
  {
    static const GTypeInfo type_info = 
    {
      sizeof (MessageClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) message_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (Message),
      0, /* n_preallocs */
      (GInstanceInitFunc) message_instance_init,
      NULL, /* value_table */
    };

    type = g_type_register_static (THRIFT_TYPE_STRUCT, 
                                   "MessageType",
                                   &type_info, 0);
  }

  return type;
}

/* constants */

