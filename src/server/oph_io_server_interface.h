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

#ifndef OPH_IO_SERVER_INTERFACE_H
#define OPH_IO_SERVER_INTERFACE_H

// error codes
#define OPH_IO_SERVER_SUCCESS                             0
#define OPH_IO_SERVER_NULL_PARAM                          1
#define OPH_IO_SERVER_MEMORY_ERROR                        2
#define OPH_IO_SERVER_ERROR                               3
#define OPH_IO_SERVER_PARSE_ERROR                         4
#define OPH_IO_SERVER_EXEC_ERROR                          5
#define OPH_IO_SERVER_METADB_ERROR                        6
#define OPH_IO_SERVER_API_ERROR                           7

//Log error codes
#define OPH_IO_SERVER_LOG_NULL_INPUT_PARAM                "Missing input argument\n"
#define OPH_IO_SERVER_LOG_MEMORY_ALLOC_ERROR              "Memory allocation error\n"
#define OPH_IO_SERVER_LOG_FILE_OPEN_ERROR                 "Error %d while opening file %s\n"
#define OPH_IO_SERVER_LOG_HASHTBL_ERROR                   "Error while creating hash table for plugins\n"
#define OPH_IO_SERVER_LOG_FILE_READ_ERROR                 "Unable to read line from %s\n"
#define OPH_IO_SERVER_LOG_PLUGIN_FILE_CORRUPTED           "Unable to read plugin file line %s\n"
#define OPH_IO_SERVER_LOG_QUERY_PARSING_ERROR             "Unable to parse query %s\n"
#define OPH_IO_SERVER_LOG_PLUGIN_LOAD_ERROR               "Unable to load plugin table\n"
#define OPH_IO_SERVER_LOG_HASHTBL_CREATE_ERROR            "Unable to create Hash table\n"
#define OPH_IO_SERVER_LOG_QUERY_ARG_LOAD_ERROR            "Unable to load query args in table\n"
#define OPH_IO_SERVER_LOG_MISSING_QUERY_ARGUMENT          "Missing argument %s in query\n"
#define OPH_IO_SERVER_LOG_QUERY_OPERATION_UNKNOWN         "Unknown input operation %s\n"
#define OPH_IO_SERVER_LOG_QUERY_METADB_ERROR              "Error querying metaDB for %s operation\n"
#define OPH_IO_SERVER_LOG_QUERY_METADB_ALLOC_ERROR        "Error creating MetaDB %s record\n"
#define OPH_IO_SERVER_LOG_QUERY_DISPATCH_ERROR            "Error dispatching query to the right operation branch\n"
#define OPH_IO_SERVER_LOG_PLUGIN_EXEC_ERROR               "Error while executing %s\n"
#define OPH_IO_SERVER_LOG_QUERY_DB_EXIST_ERROR            "DB provided already exists\n"
#define OPH_IO_SERVER_LOG_QUERY_DB_NOT_EXIST_ERROR        "DB provided does not exists\n"
#define OPH_IO_SERVER_LOG_QUERY_FRAG_EXIST_ERROR          "Frag provided already exists\n"
#define OPH_IO_SERVER_LOG_QUERY_FRAG_NOT_EXIST_ERROR      "Frag provided does not exists\n"
#define OPH_IO_SERVER_LOG_QUERY_NO_DB_SELECTED            "No DB was previously selected\n"
#define OPH_IO_SERVER_LOG_QUERY_MULTIVAL_PARSE_ERROR      "Error while parsing multivalue arg %s\n"
#define OPH_IO_SERVER_LOG_QUERY_MULTIVAL_ARGS_DIFFER      "Multivalue args for %s are not the same number\n"
#define OPH_IO_SERVER_LOG_QUERY_ENGINE_ERROR              "Error while executing engine on query %s\n"
#define OPH_IO_SERVER_LOG_QUERY_TYPE_ERROR                "Data type %s not recognized\n"
#define OPH_IO_SERVER_LOG_QUERY_INSERT_STATUS_ERROR       "Unable to perform INSERT operation due to missing table info\n"
#define OPH_IO_SERVER_LOG_QUERY_INSERT_COLUMN_ERROR       "Unable to perform INSERT: field name not found in table\n"
#define OPH_IO_SERVER_LOG_QUERY_INSERT_COLUMN_TYPE_ERROR  "Unable to perform INSERT: field type does not correspond to table\n"
#define OPH_IO_SERVER_LOG_API_SETUP_ERROR                 "Unable to setup specified device: %s\n"
#define OPH_IO_SERVER_LOG_QUERY_IO_API_ERROR              "Error while executing %s API\n"
#define OPH_IO_SERVER_LOG_LOCK_ERROR                      "Unable to execute mutex lock\n"
#define OPH_IO_SERVER_LOG_UNLOCK_ERROR                    "Unable to execute mutex unlock\n"

//Packet codes

#define OPH_IO_SERVER_MSG_TYPE_LEN 2
#define OPH_IO_SERVER_MSG_LONG_LEN sizeof(unsigned long long)
#define OPH_IO_SERVER_MSG_SHORT_LEN sizeof(unsigned int)

#define OPH_IO_SERVER_MSG_PING "PG"
#define OPH_IO_SERVER_MSG_RESULT "RS"
#define OPH_IO_SERVER_MSG_USE_DB "UD"
#define OPH_IO_SERVER_MSG_SET_QUERY "SQ"
#define OPH_IO_SERVER_MSG_EXEC_QUERY "EQ"

#define OPH_IO_SERVER_MSG_ARG_DATA_LONG "DL"
#define OPH_IO_SERVER_MSG_ARG_DATA_DOUBLE "DD"
#define OPH_IO_SERVER_MSG_ARG_DATA_NULL "DN"
#define OPH_IO_SERVER_MSG_ARG_DATA_VARCHAR "DV"
#define OPH_IO_SERVER_MSG_ARG_DATA_BLOB "DB"

#define OPH_IO_SERVER_REQ_ERROR   "ER"

// enum and struct
#define OPH_IO_SERVER_MAX_LONG_LEN 24
#define OPH_IO_SERVER_MAX_DOUBLE_LEN 32

#define OPH_IO_SERVER_BUFFER 1024

// Prototypes

#include "hashtbl.h"
#include "oph_iostorage_data.h"
#include "oph_iostorage_interface.h"
#include "oph_query_parser.h"
#include "oph_metadb_interface.h"

/**
 * \brief			            Structure to contain info about a running statemente (query executed in multiple runs)
 * \param tot_run         Total number of times the query should be executed
 * \param curr_run        Current value of execution counter
 * \param partial_result_set	Pointer to last result set retrieved by a selection query
 * \param device        	Device where result set belongs
 * \param frag       	    Frag name related to result set
 */
typedef struct{
  unsigned long long          tot_run;
  unsigned long long          curr_run;
  oph_iostore_frag_record_set *partial_result_set;
  char                        *device;
  char                        *frag;
  unsigned long long          size;
}oph_io_server_running_stmt;

/**
 * \brief			            Structure to store thread status info
 * \param current_db 	    Pointer to current (default) database, if defined
 * \param last_result_set	Pointer to last result set retrieved by a selection query
 * \param device        	Device selected for operations
 * \param curr_stmt       Current statement being executed, if any
 */
typedef struct{
  //oph_metadb_db_row *current_db; 
  char *current_db;
  oph_iostore_frag_record_set *last_result_set;
  char *device;
  oph_io_server_running_stmt *curr_stmt;
}oph_io_server_thread_status;

//Server Auxiliary functions
/**
 * \brief               Function used to dispatch query and execute the correct operation
 * \param meta_db       Pointer to metadb
 * \param thread_status Status of thread executing the query
 * \param args          Additional query arguments
 * \param query_args    Hash table containing args to be selected
 * \param plugin_table  Hash table with plugin
 * \return              0 if successfull, non-0 otherwise
 */
int oph_io_server_dispatcher(oph_metadb_db_row **meta_db, oph_iostore_handler* dev_handle, oph_io_server_thread_status *thread_status, oph_query_arg **args, HASHTBL *query_args, HASHTBL *plugin_table);

/**
 * \brief               Function used to release thread status resources
 * \param status        Thread status
 * \return              0 if successfull, non-0 otherwise
 */
int oph_io_server_free_status(oph_io_server_thread_status *status);

#endif /* OPH_IO_SERVER_INTERFACE_H */
