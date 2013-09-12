#include <json/writer.h>
#include <utility>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

#if _MSC_VER >= 1400 // VC++ 8.0
#pragma warning( disable : 4996 )   // disable warning about strdup being deprecated.
#endif
using namespace std;

namespace Json {

static void uintToString( unsigned int value,
                          char *&current )
{
   *--current = 0;
   do
   {
      *--current = (value % 10) + '0';
      value /= 10;
   }
   while ( value != 0 );
}

std::string valueToString( Value::Int value )
{
   char buffer[32];
   char *current = buffer + sizeof(buffer);
   bool isNegative = value < 0;
   if ( isNegative )
      value = -value;
   uintToString( Value::UInt(value), current );
   if ( isNegative )
      *--current = '-';
   assert( current >= buffer );
   return current;
}

std::string valueToString( Value::Int64 value )
{
	char buf[256];
	snprintf(buf, sizeof(buf), "%lld", value);
	return std::string(buf);
}

std::string valueToString( Value::UInt value )
{
   char buffer[32];
   char *current = buffer + sizeof(buffer);
   uintToString( value, current );
   assert( current >= buffer );
   return current;
}

std::string valueToString( double value )
{
   char buffer[32];
#ifdef __STDC_SECURE_LIB__ // Use secure version with visual studio 2005 to avoid warning.
   sprintf_s(buffer, sizeof(buffer), "%.2f", value);
#else
   snprintf(buffer, 32 , "%.2f", value);
#endif
   return buffer;
}


std::string valueToString( bool value )
{
   return value ? "true" : "false";
}

std::string valueToQuotedString( const char *value )
{
	if (!value) {
		return "\"\"";
	}

	std::string ret;
	ret.reserve(1024);

	ret += "\"";
	int i = 0;
	while (true)
	{
		char c = value[i++];
		if (c == 0)
			break;
		switch (c)
		{
			case '\"': ret += "\\\""; break;
			case '\\': ret += "\\\\"; break;
			case '\n': ret += "\\n"; break;
			case '\r': ret += "\\r"; break;
			case '\b': ret += "\\b"; break;
			case '\f': ret += "\\f"; break;
			case '\t': ret += "\\t"; break;
			default:
					   /*
				if (c < 0x20)
				{
					char tmp[10];
					snprintf(tmp, 9, "\\u%04x", (int)c);
					ret += tmp;
				}
				else
				{
					ret += c;
				}
				*/
					   ret += c;
				break;
		};

	}

	ret += "\"";
	return ret;
}

std::string valueToQuotedString_gbk(const char *value)
{
	std::string sString = std::string(value);
	int length = sString.length();
	char buf[length * 10 + 128];
	memset(buf, 0, sizeof(buf));

	char* it = (char*) sString.c_str();
	char *out = buf;

	for (int i = 0; i < length; i++)
	{
		// double char
		if (((unsigned char) *it > 0x80) && ((unsigned char) *it < 0xff))
		{
			if (i == (length - 1))
			{
				*out = 0; //最后一个char是双字节, 抛弃
				out++;
				it++;
			}
			else
			{
				// copy 双字节, 不过滤
				*out = *it;
				out++;
				it++;

				*out = *it;
				out++;
				it++;

				i ++;
			}

			continue;
		}

		switch (*it)
		{
			case '<':
				memcpy(out, "&lt;", strlen("&lt;"));
				out += strlen("&lt;");
				break;

			case '>':
				memcpy(out, "&gt;", strlen("&gt;"));
				out += strlen("&gt;");
				break;

			// 不过滤&, 输入的时候过滤
			/*case '&':
				memcpy(out, "&amp;", strlen("&amp;"));
				out += strlen("&amp;");
				break;*/

			case '\"':
				memcpy(out, "&quot;", strlen("&quot;"));
				out += strlen("&quot;");
				break;

			case '\'':
				memcpy(out, "&#39;", strlen("&#39;"));
				out += strlen("&#39;");
				break;

			case '\\':
				memcpy(out, "&#92;", strlen("&#92;"));
				out += strlen("&#92;");
				break;

			case '\n':
				memcpy(out, "\\n", strlen("\\n"));
				out += strlen("\\n");
				break;

			case '\r':
			case '\t':
				break;

			default:
				if ( !((*it) >= 0 && (*it) <= 31 && (*it) != 9 && (*it) != 10 && (*it) != 13))
				{
					*out = *it;
					out++;
				}
		}

		it++;
	}

	std::string retString ="\""+ std::string(buf)+"\"";
	return retString;
}


std::string valueToIdent(const char* value)
{
	if (!value)
		return "\"\"";

	const char *p = value;
	while (*p && isalnum(*p)) p++;
	if (!*p && !isdigit(*value))
		return value;
	return valueToQuotedString(value);
};



// Class FastWriter
// //////////////////////////////////////////////////////////////////
FastWriter::FastWriter()
{
	m_charset = 0;
}

std::string
FastWriter::write( const Value &root )
{
   document_ = "";
   writeValue( root );
//   document_ += "\n";
   return document_;
}

void FastWriter::write( const Value &root , std::string& strDoc)
{
   writeValue( root , strDoc);
}


void
FastWriter::writeValue( const Value &value )
{
   switch ( value.type() )
   {
   case nullValue:
      document_ += "\"\"";
      break;
   case intValue:
      document_ += valueToString( value.asInt() );
      break;
   case uintValue:
      document_ += valueToString( value.asUInt() );
      break;
   case int64Value:
	document_ += valueToString( value.asInt64() );
	break;
   case realValue:
      document_ += valueToString( value.asDouble() );
      break;
   case stringValue:
   	if(m_charset == 1)
   	{
		document_ += valueToQuotedString_gbk( value.asCString() );
   	}
	else
   	{
      document_ += valueToQuotedString( value.asCString() );
   	}
      break;
   case booleanValue:
      document_ += valueToString( value.asBool() );
      break;
   case arrayValue:
      {
         document_ += "[";
         int size = value.size();
         for ( int index =0; index < size; ++index )
         {
            if ( index > 0 )
               document_ += ",";
            writeValue( value[index] );
         }
         document_ += "]";
      }
      break;
   case objectValue:
      {
         Value::Members members( value.getMemberNames() );
         document_ += "{";
         for ( Value::Members::iterator it = members.begin();
               it != members.end();
               ++it )
         {
            const std::string &name = *it;
            if ( it != members.begin() )
               document_ += ",";
            document_ += valueToQuotedString( name.c_str() );
//            document_ += valueToIdent( name.c_str() );
            document_ += ":";
            writeValue( value[name]);
         }
         document_ += "}";
      }
      break;
   }
}

void
FastWriter::writeValue( const Value &value , std::string& stDoc )
{
   switch ( value.type() )
   {
   case nullValue:
      stDoc += "\"\"";
      break;
   case intValue:
      stDoc += valueToString( value.asInt() );
      break;
   case uintValue:
      stDoc += valueToString( value.asUInt() );
      break;
   case int64Value:
	stDoc += valueToString( value.asInt64() );
	break;
   case realValue:
      stDoc += valueToString( value.asDouble() );
      break;
   case stringValue:
   	if(m_charset == 1)
   	{
		stDoc += valueToQuotedString_gbk( value.asCString() );
   	}
	else
   	{
      stDoc += valueToQuotedString( value.asCString() );
   	}
      break;
   case booleanValue:
      stDoc += valueToString( value.asBool() );
      break;
   case arrayValue:
      {
         stDoc += "[";
         int size = value.size();
         for ( int index =0; index < size; ++index )
         {
            if ( index > 0 )
               stDoc += ",";
            writeValue( value[index]  , stDoc );
         }
         stDoc += "]";
      }
      break;
   case objectValue:
      {
         Value::Members members( value.getMemberNames() );
         stDoc += "{";
         for ( Value::Members::iterator it = members.begin();
               it != members.end();
               ++it )
         {
            const std::string &name = *it;
            if ( it != members.begin() )
               stDoc += ",";
            stDoc += valueToQuotedString( name.c_str() );
//            stDoc += valueToIdent( name.c_str() );
            stDoc += ":";
            writeValue( value[name]  , stDoc);
         }
         stDoc += "}";
      }
      break;
   }
}



// Class StyledWriter
// //////////////////////////////////////////////////////////////////

StyledWriter::StyledWriter()
   : rightMargin_( 74 )
   , indentSize_( 3 )
{
	m_charset = 0;
}


std::string
StyledWriter::write( const Value &root )
{
   document_ = "";
   addChildValues_ = false;
   indentString_ = "";
   writeCommentBeforeValue( root );
   writeValue( root );
   writeCommentAfterValueOnSameLine( root );
   document_ += "\n";
   return document_;
}


void
StyledWriter::writeValue( const Value &value )
{
   switch ( value.type() )
   {
   case nullValue:
      pushValue( "\"\"" );
      break;
   case intValue:
      pushValue( valueToString( value.asInt() ) );
      break;
   case uintValue:
      pushValue( valueToString( value.asUInt() ) );
      break;
   case int64Value:
	  pushValue( valueToString( value.asInt64() ) );
	  break;
   case realValue:
      pushValue( valueToString( value.asDouble() ) );
      break;
   case stringValue:
	  if(m_charset == 1)
	  {
		  pushValue( valueToQuotedString_gbk( value.asCString() ) );
	  }
	  else
	  {
      pushValue( valueToQuotedString( value.asCString() ) );
	  }


      //pushValue( valueToQuotedString( value.asCString() ) );
      break;
   case booleanValue:
      pushValue( valueToString( value.asBool() ) );
      break;
   case arrayValue:
      writeArrayValue( value);
      break;
   case objectValue:
      {
         Value::Members members( value.getMemberNames() );
         if ( members.empty() )
            pushValue( "{}" );
         else
         {
            writeWithIndent( "{" );
            indent();
            Value::Members::iterator it = members.begin();
            while ( true )
            {
               const std::string &name = *it;
               const Value &childValue = value[name];
               writeCommentBeforeValue( childValue );
               writeWithIndent( valueToQuotedString( name.c_str() ) );
               document_ += " : ";
               writeValue( childValue );
               if ( ++it == members.end() )
               {
                  writeCommentAfterValueOnSameLine( childValue );
                  break;
               }
               document_ += ",";
               writeCommentAfterValueOnSameLine( childValue );
            }
            unindent();
            writeWithIndent( "}" );
         }
      }
      break;
   }
}


void
StyledWriter::writeArrayValue( const Value &value )
{
   int size = value.size();
   if ( size == 0 )
      pushValue( "[]" );
   else
   {
      bool isArrayMultiLine = isMultineArray( value );
      if ( isArrayMultiLine )
      {
         writeWithIndent( "[" );
         indent();
         bool hasChildValue = !childValues_.empty();
         int index =0;
         while ( true )
         {
            const Value &childValue = value[index];
            writeCommentBeforeValue( childValue );
            if ( hasChildValue )
               writeWithIndent( childValues_[index] );
            else
            {
               writeIndent();
               writeValue( childValue );
            }
            if ( ++index == size )
            {
               writeCommentAfterValueOnSameLine( childValue );
               break;
            }
            document_ += ",";
            writeCommentAfterValueOnSameLine( childValue );
         }
         unindent();
         writeWithIndent( "]" );
      }
      else // output on a single line
      {
         assert( childValues_.size() == (unsigned int)size );
         document_ += "[ ";
         for ( int index =0; index < size; ++index )
         {
            if ( index > 0 )
               document_ += ", ";
            document_ += childValues_[index];
         }
         document_ += " ]";
      }
   }
}


bool
StyledWriter::isMultineArray( const Value &value )
{
   int size = value.size();
   bool isMultiLine = size*3 >= rightMargin_ ;
   childValues_.clear();
   for ( int index =0; index < size  &&  !isMultiLine; ++index )
   {
      const Value &childValue = value[index];
      isMultiLine = isMultiLine  ||
                     ( (childValue.isArray()  ||  childValue.isObject())  &&
                        childValue.size() > 0 );
   }
   if ( !isMultiLine ) // check if line length > max line length
   {
      childValues_.reserve( size );
      addChildValues_ = true;
      int lineLength = 4 + (size-1)*2; // '[ ' + ', '*n + ' ]'
      for ( int index =0; index < size  &&  !isMultiLine; ++index )
      {
         writeValue( value[index] );
         lineLength += int( childValues_[index].length() );
         isMultiLine = isMultiLine  &&  hasCommentForValue( value[index] );
      }
      addChildValues_ = false;
      isMultiLine = isMultiLine  ||  lineLength >= rightMargin_;
   }
   return isMultiLine;
}


void
StyledWriter::pushValue( const std::string &value )
{
   if ( addChildValues_ )
      childValues_.push_back( value );
   else
      document_ += value;
}


void
StyledWriter::writeIndent()
{
   if ( !document_.empty() )
   {
      char last = document_[document_.length()-1];
      if ( last == ' ' )     // already indented
         return;
      if ( last != '\n' )    // Comments may add new-line
         document_ += '\n';
   }
   document_ += indentString_;
}


void
StyledWriter::writeWithIndent( const std::string &value )
{
   writeIndent();
   document_ += value;
}


void
StyledWriter::indent()
{
   indentString_ += std::string( indentSize_, ' ' );
}


void
StyledWriter::unindent()
{
   assert( int(indentString_.size()) >= indentSize_ );
   indentString_.resize( indentString_.size() - indentSize_ );
}


void
StyledWriter::writeCommentBeforeValue( const Value &root )
{
   if ( !root.hasComment( commentBefore ) )
      return;
   document_ += normalizeEOL( root.getComment( commentBefore ) );
   document_ += "\n";
}


void
StyledWriter::writeCommentAfterValueOnSameLine( const Value &root )
{
   if ( root.hasComment( commentAfterOnSameLine ) )
      document_ += " " + normalizeEOL( root.getComment( commentAfterOnSameLine ) );

   if ( root.hasComment( commentAfter ) )
   {
      document_ += "\n";
      document_ += normalizeEOL( root.getComment( commentAfter ) );
      document_ += "\n";
   }
}


bool
StyledWriter::hasCommentForValue( const Value &value )
{
   return value.hasComment( commentBefore )
          ||  value.hasComment( commentAfterOnSameLine )
          ||  value.hasComment( commentAfter );
}


std::string
StyledWriter::normalizeEOL( const std::string &text )
{
   std::string normalized;
   normalized.reserve( text.length() );
   const char *begin = text.c_str();
   const char *end = begin + text.length();
   const char *current = begin;
   while ( current != end )
   {
      char c = *current++;
      if ( c == '\r' ) // mac or dos EOL
      {
         if ( *current == '\n' ) // convert dos EOL
            ++current;
         normalized += '\n';
      }
      else // handle unix EOL & other char
         normalized += c;
   }
   return normalized;
}


} // namespace Json
