#ifndef ORALIB_H_
#define ORALIB_H_

#define KBYTE	1024

#define ORALIB_SUCCESS 0
#define ORALIB_ERROR  -1
#define ORALIB_NO_MORE_DATA  1


/* Maximum number of select-list items or bind variables. */
#define ORALIB_MAX_ITEMS         40
/* Maximum SQL statement size */
#define ORALIB_MAX_SQL_SIZE	  	(4*KBYTE)

/* Maximum error size */
#define MAX_ERROR_SIZE 				(2*KBYTE)

/* Maximum lengths of the _names_ of the
   select-list items or indicator variables. */
#define ORALIB_MAX_VNAME_LEN     30
#define ORALIB_MAX_INAME_LEN     30

#ifndef NULL
#define NULL  0
#endif

#define DEBUG 
#ifdef DEBUG
#define PRINT_DEBUG printf("debug %s %d\n", __FUNCTION__, __LINE__);
#else 
#define PRINT_DEBUG printf("");
#endif

typedef enum {
	DB_FTYPE_CHAR=1, 		/* CHAR datatype */
	DB_FTYPE_NUMBER=2, 		/* NUMBER datatype, internal oracle representation */
	DB_FTYPE_INTEGER=3,		/* long long int , 8 bytes long */
	DB_FTYPE_FLOAT=4,		/* float */
	DB_FTYPE_ROWID=11, 		/* ROWID datatype */
	DB_FTYPE_LONG=8, 		/* LONG datatype */
	DB_FTYPE_DATE=12,		/* DATE datatype */
	DB_FTYPE_RAW=23,		/* RAW datatype */
	DB_FTYPE_LONGRAW=24,	/* LONG RAW datatype */
	DB_FTYPE_UROWID=104, 	/* Universal ROWID datatype */
	DB_FTYPE_UNKNOWN=-1
}OLFieldType;

typedef struct _OLField {
	int name_length;
	char *name;
	int data_length;
	void *value;
	OLFieldType type;
} OLField;

typedef struct _OL {
	void *oracle_info;
	OLField selectfields	[ORALIB_MAX_ITEMS];
	OLField bindfields	[ORALIB_MAX_ITEMS];
	char last_error		[MAX_ERROR_SIZE];
} OL;


OL *oralib_new(void);
int oralib_init(OL *db, char *username, char *password, char *database);
int oralib_alloc_descriptors(OL *db, int, int, int);
void oralib_sql_error(OL *db);
void oralib_finish(OL *db);
void oralib_destroy(OL *db);

int oralib_prepare_statement(OL *db, char *sql);

int oralib_get_num_binds(OL *db);
OLField* oralib_get_bind_field(OL *db, int num_bind);
void oralib_set_bind_field_value(OL *db, int num_bind, void *value, int length);
int oralib_get_numbind_by_name(OL *db, char *name);
void oralib_set_bind_string(OL *db, char *name, char *value);
void oralib_set_bind_long(OL *db, char *name, long value);
void oralib_set_bind_date(OL *db, char *name, char *value);

int oralib_execute(OL *db);
int oralib_commit(OL *db);
int oralib_rollback(OL *db);

int oralib_fetch(OL *db);

int oralib_get_num_fields(OL *db);
OLField* oralib_get_field(OL *db, int num_field);
OLFieldType oralib_get_field_type(OL *db, int num_field);
int oralib_get_field_size(OL *db, int num_field);
int oralib_is_field_null(OL *db, int num_field);

const char *oralib_get_field_value_as_string(OL *db, int num_field);
long long oralib_get_field_value_as_int(OL *db, int num_field);
float oralib_get_field_value_as_float(OL *db, int num_field);
const char *oralib_get_field_value_by_name_as_string(OL *db, char *name);
long long oralib_get_field_value_by_name_as_int(OL *db, char *name);
float oralib_get_field_value_by_name_as_float(OL *db, char *name);

void oralib_set_last_error(OL *db, char *error);
const char *oralib_get_last_error(OL *db);

  
 #endif /* ORALIB_H_ */
