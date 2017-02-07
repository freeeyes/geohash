/**************************************************************
 * Copyright (C), 1995-2005, SI-TECH Information Technology Co.,Ltd.
 * All Rights Reserved.
 *
 * This SOURCE CODE FILE,which has been provided by SI-TECH as part of an 
 * SI-TECH product for use ONLY by licensed users of the product, includes
 * CONFIDENTIAL and PROPRIETARY information of SI-TECH.
 *
 **************************************************************
 * Filename
 *	json.c
 *
 * Description
 *	a set of functions relate to JSON
 *
 * Version
 *	v1.00	(2016/4/27 14:06)
 *
 * Author
 *	tengyh
 *	tengyh@si-tech.com.cn
 *
 * Modification history
 * 	
 *		inition version;
 *
 *************************************************************/
#include <stdio.h>
#include <string.h>
#include "json.h"

/*******************************************************
 *	skipSpaces
 *		skip Spaces
 *
 *	Parameters
 *		pJson_data	 json  data
 *		
 *
 *	Return value
 *		
 *		
 *		
 *
 *******************************************************/
void skipSpaces(json_data *pJson_data)
{
   while ( pJson_data->current_ != pJson_data->end_ )
   {
      char c = *pJson_data->current_;
      if ( c == ' '  ||  c == '\t'  ||  c == '\r'  ||  c == '\n' )
         ++pJson_data->current_;
      else
         break;
   }
}
/*******************************************************
 *	getNextChar
 *		get next char
 *
 *	Parameters
 *		pJson_data	 json  data
 *		
 *
 *	Return value
 *		char
 *		
 *		
 *
 *******************************************************/
char getNextChar(json_data *pJson_data)
{
   if ( pJson_data->current_ == pJson_data->end_ )
      return 0;
   return *pJson_data->current_++;
}
/*******************************************************
 *	readString
 *		read String
 *
 *	Parameters
 *		pJson_data	 json  data
 *		
 *
 *	Return value
 *	=1  	success
 *	=0    fail	
 *		
 *
 *******************************************************/
int readString(json_data *pJson_data)
{
   char c = 0;
   
   while ( pJson_data->current_ != pJson_data->end_ )
   {
      c = getNextChar(pJson_data);
      if ( c == '\\' )
         getNextChar(pJson_data);
      else if ( c == '"' )
         break;
   }
   if(c == '"')
   {
   		return 1;	
   }
   else
   {
   	return 0;	
   }
   
}
/*******************************************************
 *	readToken
 *		read Token
 *
 *	Parameters
 *		pJson_data	 json  data
 *		pToken        Token  data
 *
 *	Return value
 *	=0 fail	
 *	=1 success	
 *		
 *
 *******************************************************/
int readToken(json_data *pJson_data, token_data *pToken )
{
   skipSpaces(pJson_data);
   pToken->start_ = pJson_data->current_;
   char c = getNextChar(pJson_data);
   int ok = 1;
   int type_ = 0;
   switch ( c )
   {
   case '{':
      type_ = tokenObjectBegin;
      break;
   case '}':
      type_ = tokenObjectEnd;
      break;
   case '[':
      type_ = tokenArrayBegin;
      break;
   case ']':
      type_ = tokenArrayEnd;
      break;
   case '"':
      type_ = tokenString;
      ok = readString(pJson_data);
      break;
   case '/':
      type_ = tokenComment;
      //ok = readComment();
      break;
   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
   case '-':
      type_ = tokenNumber;
      //readNumber();
      break;
   case 't':
      type_ = tokenTrue;
      //ok = match( "rue", 3 );
      break;
   case 'f':
      type_ = tokenFalse;
      //ok = match( "alse", 4 );
      break;
   case 'n':
      type_ = tokenNull;
      //ok = match( "ull", 3 );
      break;
   case ',':
      type_ = tokenArraySeparator;
      break;
   case ':':
      type_ = tokenMemberSeparator;
      break;
   case 0:
      type_ = tokenEndOfStream;
      break;
   default:
      ok = 0;
      return  0;
   }
   if ( !ok )
      type_ = tokenError;
   pToken->type_ = type_;
   pToken->end_ = pJson_data->current_;
   return 1;
}

/*******************************************************
 *	decodeString
 *		decode String
 *
 *	Parameters
 *		token	        token  data
 *		decoded        decoded  data
 *
 *	Return value
 *	=0 fail	
 *	=1 success	
 *		
 *
 *******************************************************/

int decodeString( token_data &token, char *decoded )
{
   //decoded.reserve( token.end_ - token.start_ - 2 );
   char * current = token.start_ + 1; // skip '"'
   char* end = token.end_ - 1;      // do not include '"'
   int i = 0;
   while ( current != end )
   {
      char c = *current++;
      if ( c == '"' )
         break;
      else
      {
         decoded[i]= c;
         i++;
      }
   }
   return 1;
}
# if 0
int readObject( json_data *pJson_data,token_data &tokenStart )
{
   token_data tokenName;
   char name[512];
   //currentValue() = Value( objectValue );
   while ( readToken( pJson_data,&tokenName ) )
   {
      int initialTokenOk = 1;
      if(tokenName.type_ == tokenComment)
      	break;
      //while (/* tokenName.type_ == tokenComment  && */ initialTokenOk )
         //initialTokenOk = readToken( tokenName );
      if  ( !initialTokenOk )
         break;
      if ( tokenName.type_ == tokenObjectEnd /* &&  name.empty()*/ )  // empty object
         return 1;
      if ( tokenName.type_ != tokenString )
         break;
      
      memset(name,0,sizeof(name);
      if ( !decodeString( tokenName, name ) )
      {
      	printf("readObject decodeString error\n");
      	return 0;
			}
      Token colon;
      if ( !readToken( colon ) ||  colon.type_ != tokenMemberSeparator )
      {
         printf("Missing ':' after object member name");
         return 0;
      }
      //Value &value = currentValue()[ name ];
      //nodes_.push( &value );
      int ok = readValue(pJson_data);
      //nodes_.pop();
      if ( !ok ) // error already set
       {
       		printf("readObject readValue error\n");
       	  return 0;
       }

      Token comma;
      if ( !readToken( comma )
            ||  ( comma.type_ != tokenObjectEnd  &&  
                  comma.type_ != tokenArraySeparator &&
		  comma.type_ != tokenComment ) )
      {
         printf("Missing ',' or '}' in object declaration\n");
         return 0;
      }
      bool finalizeTokenOk = true;
      while ( comma.type_ == tokenComment &&
              finalizeTokenOk )
         finalizeTokenOk = readToken( comma );
      if ( comma.type_ == tokenObjectEnd )
         return 1;
   }
   printf("Missing '}' or object member name\n");
   return 0;
   
}
//#endif
int readValue(json_data *pJson_data)
{
   token_data token;
   //skipCommentTokens( token );
   int successful = 0;

   readToken(pJson_data,&token);


   switch ( token.type_ )
   {
   case tokenObjectBegin:
      successful = readObject(pJson_data,&token );
      break;
   case tokenArrayBegin:
      //successful = readArray( token );
      break;
   case tokenNumber:
      //successful = decodeNumber( token );
      break;
   case tokenString:
      successful = decodeString( token );
      break;
   case tokenTrue:
     // currentValue() = true;
      break;
   case tokenFalse:
     // currentValue() = false;
      break;
   case tokenNull:
      //currentValue() = Value();
      break;
   default:
      printf( "Syntax error: token.type_[%d] \n", token.type_ );
      return 0;
   }

   

   return successful;
}
#endif
/*******************************************************
 *	jsonReadValue
 *		Read json  Value
 *
 *	Parameters
 *		src	        src  data
 *		object      json  object
 *    value       json  object  value
 *    pJson_data  Json data
 *    isBegin     begin or not
 *	Return value
 *	=0 fail	
 *	=1 success	
 *		
 *
 *******************************************************/
int jsonReadValue(char *src,char *object,char *value,json_data *pJson_data,int isBegin)
{
	//token_data token;
	if((!src)||(!object)||(!value)||(!pJson_data))
	{
		printf("\njsonReadValue para is null\n");	
		return 0;
	}
	//json_data sJson_data;
	//sJson_data.begin_ = src;
	//sJson_data.end_ = src + strlen(src);
	//sJson_data.current_ = src;
	//int successful = readValue(pJson_data);
		token_data tokenName;
		token_data tokenValue;
  	int initialTokenOk = 0;
   while ( readToken( pJson_data,&tokenName ) )
   {
      
      if((tokenName.type_ == tokenComment)||(tokenName.type_ == tokenEndOfStream))
      	break;
      //while (/* tokenName.type_ == tokenComment  && */ initialTokenOk )
         //initialTokenOk = readToken( tokenName );
      //if  ( !initialTokenOk )
         //break;
      if ( tokenName.type_ == tokenObjectEnd/* &&  name.empty()*/ )  // empty object
         return 2;
      if ( tokenName.type_ == tokenObjectBegin )
      {
      	initialTokenOk = 1;
      	continue;
      }
      if((0 == initialTokenOk)&&(isBegin))
      {
      	continue;
      }
      
      if ( tokenName.type_ != tokenString )
         break;
      
      //memset(name,0,sizeof(name);
      if ( !decodeString( tokenName, object ) )
      {
      	printf("readObject decodeString error\n");
      	return 0;
			}
      token_data colon;
      if ( !readToken( pJson_data,&colon ) )
      {
         printf("readToken Missing ':' after object member name");
         return 0;
      }
      if ( colon.type_ != tokenMemberSeparator )//
      {
         printf("Missing ':' after object member name\n");
         return 0;
      }
      if ( !readToken( pJson_data,&tokenValue ) )
      {
         printf("read tokenValue error\n");
         return 0;
      }
      if ( tokenName.type_ != tokenString )
         break;
      
			//memset(value,0,sizeof(value);
      if ( !decodeString( tokenValue, value ) )
      {
      	printf("readObject decodeString error value\n");
      	return 0;
			}
      token_data comma;
      if ( !readToken( pJson_data,&comma )
            ||  ( comma.type_ != tokenObjectEnd  &&  
                  comma.type_ != tokenArraySeparator)  )
      {
         printf("Missing ',' or '}' in object declaration\n");
         return 0;
      }
      else
      {
      	 if(comma.type_ == tokenObjectEnd)
      	 {
      	 	return 2;	
      	 }
      	 
         return 1;
        
       }
   }
   printf("Missing '}' or object member name\n");
   return 0;
}
/*******************************************************
 *	jsonWriteBegin
 *		Write first json  data
 *
 *	Parameters
 *		buff	        json  buff
 *		name          json  object
 *    value         json  object  value
 *	Return value
 *	=0 fail	
 *	=1 success	
 *		
 *
 *******************************************************/
int jsonWriteBegin(char * buff,char *name,char *value)
{
	if((!buff)||(!name)	||(!value))
	{
		return 0;	
	}
	buff[0] = '{';
	buff[1] = '"';
	strcat(buff,name);
	buff[strlen(buff)] = '"';
	buff[strlen(buff)] = ':';
	buff[strlen(buff)] = '"';
	strcat(buff,value);
	buff[strlen(buff)] = '"';
	return 1;
}
/*******************************************************
 *	jsonWriteNext
 *		Write next json  data
 *
 *	Parameters
 *		buff	        json  buff
 *		name          json  object
 *    value         json  object  value
 *	Return value
 *	=0 fail	
 *	=1 success	
 *		
 *
 *******************************************************/
int jsonWriteNext(char * pBuff,char *name,char *value)
{
	char * buff = NULL;
	if((!pBuff)||(!name)	||(!value))
	{
		return 0;	
	}
	buff = pBuff + strlen(pBuff);
	buff[0] = ',';
	buff[1] = '"';
	strcat(buff,name);
	buff[strlen(buff)] = '"';
	buff[strlen(buff)] = ':';
	buff[strlen(buff)] = '"';
	strcat(buff,value);
	buff[strlen(buff)] = '"';
	return 1;
}
/*******************************************************
 *	jsonWriteEnd
 *		Write  json  data   terminator
 *
 *	Parameters
 *		buff	        json  buff
 *		name          json  object
 *    value         json  object  value
 *	Return value
 *	=0 fail	
 *	=1 success	
 *		
 *
 *******************************************************/
int jsonWriteEnd(char * buff)
{
	char *pBuff = NULL;
	if(!buff)
	{
		return 0;	
	}
	pBuff = buff + strlen(buff);
	pBuff[0] = '}';
	return 1;
	
}

