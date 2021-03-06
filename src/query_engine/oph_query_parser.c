/*
    Ophidia IO Server
    Copyright (C) 2014-2016 CMCC Foundation

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _GNU_SOURCE

#include "oph_query_parser.h"
#include "oph_query_engine_log_error_codes.h"
#include "oph_query_engine_language.h"

#include <stdlib.h>
#include <stdio.h>
#include <ltdl.h>
#include <string.h>

#include <ctype.h>
#include <unistd.h>

#include <debug.h>

extern int msglevel;

//Auxiliary parser functions 
int _oph_query_parser_validate_query(const char *query_string)
{
  if (!query_string){
    pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_NULL_INPUT_PARAM);
  	logging(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_NULL_INPUT_PARAM);    
    return OPH_QUERY_ENGINE_NULL_PARAM;
  }

  //Track the last char
  char previous_char = 0;
  int i;

  int delim_number = 0;

  for (i = 0; query_string[i]; i++){
	  switch(query_string[i]){
		  case OPH_QUERY_ENGINE_LANG_PARAM_SEPARATOR:{ 
			  if((previous_char == OPH_QUERY_ENGINE_LANG_PARAM_SEPARATOR || previous_char == OPH_QUERY_ENGINE_LANG_MULTI_VALUE_SEPARATOR)){
          pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);
        	logging(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);   
				  return OPH_QUERY_ENGINE_PARSE_ERROR;
        }
			  break;
		  }
		  case OPH_QUERY_ENGINE_LANG_MULTI_VALUE_SEPARATOR:{
			  if((previous_char == OPH_QUERY_ENGINE_LANG_PARAM_SEPARATOR || previous_char == OPH_QUERY_ENGINE_LANG_MULTI_VALUE_SEPARATOR)){
          pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);
        	logging(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);   
				  return OPH_QUERY_ENGINE_PARSE_ERROR;
        }
			  break;
		  }			
		  case OPH_QUERY_ENGINE_LANG_STRING_DELIMITER:{
        delim_number++;
			  break;
		  }			
	  }
	  previous_char = query_string[i];
  }
  //Check that number of string delimiters is even
  if(delim_number % 2 == 1){
    pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);
  	logging(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);    
	  return OPH_QUERY_ENGINE_PARSE_ERROR;
  }

  return OPH_QUERY_ENGINE_SUCCESS;
}

int _oph_query_parser_load_query_params(const char *query_string, HASHTBL *hashtbl)
{
  if (!query_string || !hashtbl){
    pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_NULL_INPUT_PARAM);
  	logging(LOG_ERROR, __FILE__, __LINE__,OPH_QUERY_ENGINE_LOG_NULL_INPUT_PARAM);    
    return OPH_QUERY_ENGINE_NULL_PARAM;
  }

  const char *ptr_begin, *ptr_equal, *ptr_end;

  ptr_begin = query_string;
  ptr_equal = strchr(query_string, OPH_QUERY_ENGINE_LANG_VALUE_SEPARATOR);
  ptr_end = strchr(query_string, OPH_QUERY_ENGINE_LANG_PARAM_SEPARATOR);

  char param[OPH_QUERY_ENGINE_LANG_LEN] = {'\0'};
  char value[OPH_QUERY_ENGINE_LANG_LEN] = {'\0'};
  char *real_val = NULL;

  while(ptr_end)
  {
    if(!ptr_begin || !ptr_equal || !ptr_end ){
      pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);
    	logging(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);   
		  return OPH_QUERY_ENGINE_PARSE_ERROR;
    }

    if(ptr_end < ptr_equal){
      pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);
    	logging(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);    
		  return OPH_QUERY_ENGINE_PARSE_ERROR;
    }

    strncpy(param,ptr_begin, strlen(ptr_begin) - strlen(ptr_equal));
	  param[strlen(ptr_begin) - strlen(ptr_equal)] = 0;	
   
    strncpy(value,ptr_equal + 1, strlen(ptr_equal + 1) - strlen(ptr_end));
	  value[strlen(ptr_equal + 1) - strlen(ptr_end)] = 0;	

	  real_val = (char*)strndup(value, (strlen(value) + 1)*sizeof(char));
	  if(real_val == NULL)
	  {
      pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_MEMORY_ALLOC_ERROR);
		  logging(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_MEMORY_ALLOC_ERROR);
      return OPH_QUERY_ENGINE_MEMORY_ERROR;
    }

    hashtbl_insert(hashtbl, (char *)param, (char *)real_val);
    real_val = NULL;
    ptr_begin = ptr_end + 1;
    ptr_equal = strchr(ptr_end + 1, OPH_QUERY_ENGINE_LANG_VALUE_SEPARATOR);
    ptr_end = strchr(ptr_end + 1, OPH_QUERY_ENGINE_LANG_PARAM_SEPARATOR);
  }

  return OPH_QUERY_ENGINE_SUCCESS;
}

char *multival_strchr(char * str){

  int i = 0;
  int string_flag = 0;
  for (i = 0; str[i]; i++){
    //In this case set the flag for an opening string delimiter
    if(str[i] == OPH_QUERY_ENGINE_LANG_STRING_DELIMITER && !string_flag) string_flag = 1;
    //Count separator only if not in the middle of a string
  	else if(str[i] == OPH_QUERY_ENGINE_LANG_MULTI_VALUE_SEPARATOR && !string_flag) return &(str[i]);
    //If string is open, then the new delimiter will close it
    else if(str[i] == OPH_QUERY_ENGINE_LANG_STRING_DELIMITER && string_flag) string_flag = 0;
  }
  return NULL;
}

int oph_query_parse_multivalue_arg (char *values, char ***value_list, int *value_num)
{
  if (!values || !value_list || !value_num){
    pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_NULL_INPUT_PARAM);
  	logging(LOG_ERROR, __FILE__, __LINE__,OPH_QUERY_ENGINE_LOG_NULL_INPUT_PARAM);    
    return OPH_QUERY_ENGINE_NULL_PARAM;
  }
  
  int param_num = 1;
  int j,i;

  *value_list = NULL;

  //Count number of parameters. Do not count those enclosed by ''
  int string_flag = 0;
  for (i = 0; values[i]; i++){
    //In this case set the flag for an opening string delimiter
    if(values[i] == OPH_QUERY_ENGINE_LANG_STRING_DELIMITER && !string_flag) string_flag = 1;
    //Count separator only if not in the middle of a string
  	else if(values[i] == OPH_QUERY_ENGINE_LANG_MULTI_VALUE_SEPARATOR && !string_flag) param_num++;
    //If string is open, then the new delimiter will close it
    else if(values[i] == OPH_QUERY_ENGINE_LANG_STRING_DELIMITER && string_flag) string_flag = 0;
  }


  *value_list = (char **)malloc(param_num*sizeof(char*));
  if(!(*value_list)){
    pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_MEMORY_ALLOC_ERROR);
  	logging(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_MEMORY_ALLOC_ERROR);   
		return OPH_QUERY_ENGINE_MEMORY_ERROR;
  }

  char *ptr_begin, *ptr_end;

  ptr_begin = values;
  ptr_end = multival_strchr(values);
  j = 0;
  while(ptr_begin)
  { 
	  if(ptr_end){
		  (*value_list)[j] = ptr_begin;
		  (*value_list)[j][strlen(ptr_begin) - strlen(ptr_end) ] = 0;
		  ptr_begin = ptr_end + 1;
		  ptr_end = multival_strchr(ptr_end + 1);
	  }
	  else{
		  (*value_list)[j] = ptr_begin;
		  (*value_list)[j][strlen(ptr_begin)] = 0;
		  ptr_begin = NULL;
	  }
	  j++;
  }

  *value_num = param_num;
  return OPH_QUERY_ENGINE_SUCCESS;
}

int _oph_query_check_query_params(HASHTBL *hashtbl)
{
  if (!hashtbl){
	pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_NULL_INPUT_PARAM);
	logging(LOG_ERROR, __FILE__, __LINE__,OPH_QUERY_ENGINE_LOG_NULL_INPUT_PARAM);    
	return OPH_QUERY_ENGINE_NULL_PARAM;
  }

  if( hashtbl_get(hashtbl, OPH_QUERY_ENGINE_LANG_ARG_FROM_ALIAS) || hashtbl_get(hashtbl, OPH_QUERY_ENGINE_LANG_ARG_WHEREL) || hashtbl_get(hashtbl, OPH_QUERY_ENGINE_LANG_ARG_WHEREC) || hashtbl_get(hashtbl, OPH_QUERY_ENGINE_LANG_ARG_WHERER) || hashtbl_get(hashtbl, OPH_QUERY_ENGINE_LANG_ARG_GROUP) || hashtbl_get(hashtbl, OPH_QUERY_ENGINE_LANG_ARG_WHERE) || hashtbl_get(hashtbl, OPH_QUERY_ENGINE_LANG_ARG_FUNC) || hashtbl_get(hashtbl, OPH_QUERY_ENGINE_LANG_ARG_ARG)){
	pmesg(LOG_ERROR, __FILE__, __LINE__, "Query not valid: keyword not supported.\n");
	logging(LOG_ERROR, __FILE__, __LINE__, "Query not valid: keyword not supported.\n");
	return OPH_QUERY_ENGINE_PARSE_ERROR;
  }

  char* tmp = hashtbl_get(hashtbl, OPH_QUERY_ENGINE_LANG_ARG_ORDER_DIR);
  if ( tmp && strcasecmp(tmp,"ASC") ) {
	pmesg(LOG_WARNING, __FILE__, __LINE__, "Keyword '%s' skipped.\n", OPH_QUERY_ENGINE_LANG_ARG_ORDER_DIR);
	logging(LOG_WARNING, __FILE__, __LINE__, "Keyword '%s' skipped.\n", OPH_QUERY_ENGINE_LANG_ARG_ORDER_DIR);
  }

  return OPH_QUERY_ENGINE_SUCCESS;
}

int oph_query_parser(char *query_string, HASHTBL **query_args)
{
	if (!query_string || !query_args){	
    pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_NULL_INPUT_PARAM);
  	logging(LOG_ERROR, __FILE__, __LINE__,OPH_QUERY_ENGINE_LOG_NULL_INPUT_PARAM);    
    return OPH_QUERY_ENGINE_NULL_PARAM;
	}

	//Check if string has correct format
	if(_oph_query_parser_validate_query(query_string)){
    pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);
  	logging(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);   
	  return OPH_QUERY_ENGINE_PARSE_ERROR;
	}

  //Create hash table for arguments
  *query_args = NULL;
  
	if( !(*query_args = hashtbl_create(OPH_QUERY_ENGINE_QUERY_ARGS, NULL)) ){
    pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_HASHTBL_CREATE_ERROR);
  	logging(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_HASHTBL_CREATE_ERROR);   
		return OPH_QUERY_ENGINE_ERROR;
	}

	//Split all arguments and load each one into hast table
	if(_oph_query_parser_load_query_params(query_string, *query_args) ){
    pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_ARG_LOAD_ERROR);
  	logging(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_ARG_LOAD_ERROR);   
    hashtbl_destroy(*query_args);
    *query_args = NULL;
    return OPH_QUERY_ENGINE_PARSE_ERROR;
	}

	//Check supported keywords
	if(_oph_query_check_query_params(*query_args) ){
    pmesg(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);
  	logging(LOG_ERROR, __FILE__, __LINE__, OPH_QUERY_ENGINE_LOG_QUERY_PARSING_ERROR, query_string);   
    hashtbl_destroy(*query_args);
    *query_args = NULL;
    return OPH_QUERY_ENGINE_PARSE_ERROR;
	}

  return OPH_QUERY_ENGINE_SUCCESS;
}
