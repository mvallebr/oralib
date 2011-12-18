#include <oralib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void sql_error(void);
int get_dyn_statement(void);
void help(void);

char    dyn_statement[ORALIB_MAX_SQL_SIZE];

int main() {
	int i, num_binds;
	OLField* field;
	char bind_value[64];
	
	OL *db = oralib_new();
	oralib_init(db, "YOUR_USER", "YOUR_PASSWORD", "YOUR_SID");

	while (1)	{
		/* Get the statement.  Break on "exit". */
		if (get_dyn_statement() != 0)
			break;
		printf("dyn_statement='%s'\n", dyn_statement);
		
		if (oralib_prepare_statement(db, dyn_statement)!=ORALIB_SUCCESS) {
			fprintf(stderr, "Error when trying to prepare statement! \n%s\n", oralib_get_last_error(db));
			continue;
		}
		
		num_binds = oralib_get_num_binds (db);
		for (i=0; i<num_binds; i++) { 
			field = oralib_get_bind_field(db, i);
			printf("Field [%d] Please, enter the value for the field '%.*s': ", i, field->name_length, field->name);
			fgets(bind_value, sizeof(bind_value), stdin);
			if (strlen(bind_value)>0)
				bind_value[strlen(bind_value)-1]='\0';
			oralib_set_bind_string(db, field->name, bind_value);
		}

		int execute_result = oralib_execute(db);
		/* if the statement is a select, fetch the results */
		if (strncasecmp(dyn_statement, "SELECT", 6) == 0)  {			
			int result, i, num_fields;
			
			if (execute_result!=ORALIB_SUCCESS) {
				fprintf(stderr, "Error when trying to execute SELECT statement! \n%s\n",oralib_get_last_error(db));
				continue;
			} else {
				printf("SELECT statement executed successfully! \n");
			}
			
			printf("SELECT RESULT:\n\n");
			while (1) {
				result = oralib_fetch(db);
				if (result==ORALIB_ERROR) {
					fprintf(stderr, "Error when trying to fetch select result! \n%s\n", oralib_get_last_error(db));
					break;
				}
				if (result==ORALIB_NO_MORE_DATA)
					break;
				/* Use record */ 
				num_fields=oralib_get_num_fields(db);
				printf("\n");
				for (i=0; i<num_fields; i++) {
					printf("\t");
					OLFieldType type = oralib_get_field_type(db, i);
					if (oralib_is_field_null(db, i)) {
						printf("NULL");
						continue;
					}
					/* printf("type=%d\t", type); */
					switch (type) {
						case DB_FTYPE_INTEGER:
							printf("%*ld", oralib_get_field_size(db, i), (long int)oralib_get_field_value_as_int(db, i));
							break;
						case DB_FTYPE_FLOAT:
							printf("%f", oralib_get_field_value_as_float(db, i));
							break;
						default:
							printf("%s", oralib_get_field_value_as_string(db, i));
							break;					
					}
				}							
			} 			
			printf("\n");				
		} else { /*update delete insert */
			if (execute_result!=ORALIB_SUCCESS) {
				oralib_rollback(db);
				fprintf(stderr, "Error when trying to execute statement! \n%s\n", oralib_get_last_error(db));
				continue;
			} else {
				oralib_commit(db);
				printf("Statement executed successfully! \n");
			}
		}
	}
			
	oralib_finish(db);
	oralib_destroy(db);
}


int get_dyn_statement()
{
    char *cp, linebuf[256];
    int iter, plsql;


    for (plsql = 0, iter = 1; ;)
    {
        if (iter == 1)
        {
            printf("\nSQL> ");
            dyn_statement[0] = '\0';
        }
        
        fgets(linebuf, sizeof linebuf, stdin);

        cp = strrchr(linebuf, '\n');
        if (cp && cp != linebuf)
            *cp = ' ';
        else if (cp == linebuf)
            continue;

        if ((strncmp(linebuf, "EXIT", 4) == 0) ||
            (strncmp(linebuf, "exit", 4) == 0))
        {
            return -1;
        }

        else if (linebuf[0] == '?' ||
            (strncmp(linebuf, "HELP", 4) == 0) ||
            (strncmp(linebuf, "help", 4) == 0))
        {
            help();
            iter = 1;
            continue;
        }

        if (strstr(linebuf, "BEGIN") ||
            (strstr(linebuf, "begin")))
        {
            plsql = 1;
        }

        strcat(dyn_statement, linebuf);

        if ((plsql && (cp = strrchr(dyn_statement, '/'))) ||
            (!plsql && (cp = strrchr(dyn_statement, ';'))))
        {
            *cp = '\0';
            break;
        }
        else
        {
            iter++;
            printf("%3d  ", iter);
        }
    }
    return 0;
}



void help()
{
    puts("\n\nEnter a SQL statement or a PL/SQL block at the SQL> prompt.");
    puts("Statements can be continued over several lines, except");
    puts("within string literals.");
    puts("Terminate a SQL statement with a semicolon.");
    puts("Terminate a PL/SQL block (which can contain embedded semicolons)");
    puts("with a slash (/).");
    puts("Typing \"exit\" (no semicolon needed) exits the program.");
    puts("You typed \"?\" or \"help\" to get this message.\n\n");
}


