/*
  Chicken Scheme MariaDB Connector
  (c) 12-25-2023 github.com/nobody246 Alex S. a.se8seven(a)gm
  Released under LGPL v2 or MIT (your choice of either) 
  I'm not responsible for anything.
*/

#include <mysql/mysql.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>


MYSQL* con = NULL;
MYSQL_STMT* stmt = NULL;
MYSQL_BIND* bnd = NULL;
MYSQL_RES* resMetaData = NULL;
unsigned int resFieldCount = 0;
MYSQL_BIND* resBnd = NULL;
MYSQL_FIELD* resFld = NULL;
char* chMDBErrString;
char* chMDBErrNo;
unsigned int stmtFieldCount = 0;
unsigned int resStat = 0;
unsigned int resRowCount = 0;

enum enum_field_types* paramTypeOrder = NULL;
enum enum_field_types* resTypeOrder = NULL;

int paramStringInd = 0;
int resultColStringInd = 0;
char** paramStringVal = NULL;
char** resultColStringVal = NULL;
unsigned long* paramStringLen = NULL;
unsigned long* resultColStringLen = NULL;
my_bool* paramStringNull = NULL;
my_bool* resultColStringNull = NULL;

int paramTinyIntInd = 0;
int resultColTinyIntInd = 0;
signed char* paramTinyIntVal = NULL;
signed char* resultColTinyIntVal = NULL;
my_bool* paramTinyIntNull = NULL;
my_bool* resultColTinyIntNull = NULL;

int paramShortIntInd = 0;
int resultColShortIntInd = 0;
short int* paramShortIntVal = NULL;
short int* resultColShortIntVal = NULL;
my_bool* paramShortIntNull = NULL;
my_bool* resultColShortIntNull = NULL;

int paramIntInd = 0;
int resultColIntInd = 0;
int* paramIntVal = NULL;
int* resultColIntVal = NULL;
my_bool* paramIntNull = NULL;
my_bool* resultColIntNull = NULL;

int paramLongIntInd = 0;
int resultColLongIntInd = 0;
long long int* paramLongIntVal = NULL;
long long int* resultColLongIntVal = NULL;
my_bool* paramLongIntNull = NULL;
my_bool* resultColLongIntNull = NULL;

int paramDoubleInd = 0;
int resultColDoubleInd = 0;
double* paramDoubleVal = NULL;
double* resultColDoubleVal = NULL;
my_bool* paramDoubleNull = NULL;
my_bool* resultColDoubleNull = NULL;

int paramFloatInd = 0;
int resultColFloatInd = 0;
float* paramFloatVal = NULL;
float* resultColFloatVal = NULL;
my_bool* paramFloatNull = NULL;
my_bool* resultColFloatNull = NULL;

int paramDateTimeInd = 0;
unsigned int*  paramYear = 0;
unsigned int*  paramMonth  = 0;
unsigned int*  paramDay  = 0;
unsigned int*  paramHour  = 0;
unsigned int*  paramMinute  = 0;
unsigned int*  paramSecond  = 0;
MYSQL_TIME*    paramMSTime  = {0};
my_bool*       paramDateTimeNull  = NULL;  

int resultColDateTimeInd = 0;
unsigned int*  resultColYear  = NULL;
unsigned int*  resultColMonth = NULL;
unsigned int*  resultColDay = NULL;
unsigned int*  resultColHour = NULL;
unsigned int*  resultColMinute = NULL;
unsigned int*  resultColSecond = NULL;
MYSQL_TIME*    resultColMSTime = NULL;
my_bool*       resultColDateTimeNull = NULL;  

MYSQL_FIELD* resFields = NULL;
unsigned int bindCount = 0;
unsigned int resBindCount = 0;
char* sqlStr;




unsigned int chMDBAddTypeOrderEntry(enum enum_field_types** fldType,
				    enum enum_field_types val,
				    unsigned int ind)
{
  if (!ind)
    {
      (*fldType) =
	(enum enum_field_types*) calloc(1,
					sizeof(enum enum_field_types));
    }
  else
    {
      enum enum_field_types* ty =
	(enum enum_field_types*) realloc((*fldType),
					 (ind + 1) *
					 sizeof(enum enum_field_types));
      if (!ty)
	{return 0;}
      (*fldType) = ty;
    }
  (*fldType)[ind] = (enum enum_field_types) val;
  return 1;
}

unsigned int chMDBClearTypeOrderMemory(enum enum_field_types** fldType)
{
  if (!(*fldType))
    {return 0;}
  free((*fldType));
  (*fldType) = NULL;
  return 1;
}

void chMDBClearDateTimeMemory(int* ind,
			      MYSQL_TIME** MSTimeL,
			      my_bool** nullL)
{
  if ((*MSTimeL))
    {
      free(*MSTimeL);
      (*MSTimeL) = NULL;
    }
  if ((*nullL))
    {
      free((*nullL));
      (*nullL) = NULL;
    }
  (*ind) = 0;
}

void chMDBClearStringMemory(int* ind,
			    char*** valL,
			    unsigned long** stringLenL,
			    my_bool** nullL)
{
  if (!(*valL))
    {return;}
  for(int i = 0; i<(*ind); i++)
    {
      if ((*valL)[i])
	{
	  free((*valL)[i]);
	  (*valL)[i] = NULL;
	}
    }
  free((*stringLenL));
  (*stringLenL) = NULL;
  (*ind) = 0;
  free((*valL));
  (*valL) = NULL;
  free((*nullL));
  (*nullL) = NULL;
}

void chMDBClearTinyIntMemory(int* ind,
			     signed char** valL,
			     my_bool** nullL)
{
  if (!(*ind) || !(*valL))
    {return;}
  (*ind) = 0;
  free((*valL));
  (*valL) = NULL;
  free((*nullL));
  (*nullL) = NULL;
}

void chMDBClearShortIntMemory(int* ind,
			      short int** valL,
			      my_bool** nullL)
{
  if (!(*ind) || !(*valL))
    {return;}
  (*ind) = 0;  
  free((*valL));
  (*valL) = NULL;
  free((*nullL));
  (*nullL) = NULL;
}

void chMDBClearIntMemory(int* ind,
			 int** valL,
			 my_bool** nullL)
{
  if (!(*ind) || !(*valL))
    {return;}
  (*ind) = 0;  
  free((*valL));
  (*valL) = NULL;
  free((*nullL));
  (*nullL) = NULL;
}

void chMDBClearLongIntMemory(int* ind,
			     long long int** valL,
			     my_bool** nullL)
{
  if (!(*ind) || !(*valL))
    {return;}
  (*ind) = 0;  
  free((*valL));
  (*valL) = NULL;
  free((*nullL));
  (*nullL) = NULL;
}

void chMDBClearDoubleMemory(int* ind,
			    double** valL,
			    my_bool** nullL)
{
  if (!(*ind) || !(*valL))
    {return;}
  (*ind) = 0;
  free((*valL));
  (*valL) = NULL;
  free((*nullL));
  (*nullL) = NULL;
}

void chMDBClearFloatMemory(int* ind,
			   float** valL,
			   my_bool** nullL)
{
  if (!(*ind) || !(*valL))
    {return;}
  (*ind) = 0;  
  free((*valL));
  (*valL) = NULL;
  free((*nullL));
  (*nullL) = NULL;
}

void chMDBClearBindMemory(int* ind,
			  MYSQL_BIND** valL)
{
  if (!(*ind) || !(*valL))
    {return;}
  free((*valL));
  (*ind) = 0;  
  (*valL) = NULL;
}

void cleanup()
{
  if (bnd)
    {
      chMDBClearBindMemory(&bindCount,
			   &bnd);
    }
  if (stmt)
    {
      mysql_stmt_close(stmt);
      stmt = NULL;
    }
  resRowCount = 0;
  resFieldCount = 0;
  if (resBnd)
    {
      chMDBClearBindMemory(&resBindCount,
			   &resBnd);
      if (resMetaData)
	{
	  mysql_free_result(resMetaData);
	  resMetaData = NULL;
	  resFld = NULL;
	}
    }
  chMDBClearDateTimeMemory(&paramDateTimeInd,
			   &paramMSTime,
			   &paramDateTimeNull);
  chMDBClearStringMemory(&paramStringInd,
			 &paramStringVal,
			 &paramStringLen,
			 &paramStringNull);
  chMDBClearTinyIntMemory(&paramTinyIntInd,
			  &paramTinyIntVal,
			  &paramTinyIntNull);
  chMDBClearShortIntMemory(&paramShortIntInd,
			   &paramShortIntVal,
			   &paramShortIntNull);
  chMDBClearIntMemory(&paramIntInd,
		      &paramIntVal,
		      &paramIntNull);
  chMDBClearLongIntMemory(&paramLongIntInd,
			  &paramLongIntVal,
			  &paramLongIntNull);
  chMDBClearDoubleMemory(&paramDoubleInd,
			 &paramDoubleVal,
			 &paramDoubleNull);
  chMDBClearFloatMemory(&paramFloatInd,
			&paramFloatVal,
			&paramFloatNull);
  if (paramTypeOrder)
    {chMDBClearTypeOrderMemory(&paramTypeOrder);}
  if (resTypeOrder)
    {chMDBClearTypeOrderMemory(&resTypeOrder);}
  
  if (sqlStr)
  {
    free(sqlStr);
    sqlStr = NULL;
  }

}

unsigned char allocateStringMemory(int ind,
				   char*** val,
				   unsigned long** len,
				   my_bool** nul)
{
  if (!ind)
    {
      (*val) = (char**) calloc(1,
			       sizeof(char*));
      (*len) = (unsigned long*) calloc(1,
				       sizeof(unsigned long));
      (*nul) = (my_bool*) calloc(1,
				sizeof(my_bool));
      if (!(*val) || !(*len) || !(*nul))
	{
	  if (*val)
            {free(*val);}
	  if (*len)
            {free(*len);}
	  if (*nul)
	    {free(*nul);}
	  return 0;
	}
    }
  else
    {
      char** y = (char **) realloc((*val),
				   (ind + 1) * sizeof(char*));
      unsigned long* z = (unsigned long*) realloc((*len),
						  (ind + 1) *
   				                  sizeof(unsigned long));
      my_bool* w = (my_bool*) realloc((*nul),
				      (ind + 1) * sizeof(my_bool));
      if (!y || !z || !w)
	{
	  if (z)
            {free(z);}
	  if (y)
            {free(y);}
	  if (w)
	    {free(w);}
	  return 0;
	}
      (*val) = y;
      (*len) = z;
      (*nul) = w;
      ((*val)[ind]) = (char) 0;
      ((*len)[ind]) = (unsigned long) 0;
      ((*nul)[ind]) = (my_bool) 0;
    }
  return 1;
}

unsigned char allocateDateTimeMemory(int            ind,
				     MYSQL_TIME**   MSTime,
				     my_bool**      nul)
{

  if (!ind)
    {
      (*MSTime) = (MYSQL_TIME*) calloc(1, sizeof(MYSQL_TIME));
      (*nul) = (my_bool*) calloc(1, sizeof(my_bool));
    }
  else
    {
      MYSQL_TIME* a = realloc((*MSTime),
			      (ind + 1) * sizeof(MYSQL_TIME));
      my_bool*    b = realloc((*nul),
			      (ind + 1) *  sizeof(my_bool));
      if (!a || !b)
	{return 0;}
      (*MSTime) = a;
      (*nul) = b;
      memset(&(*MSTime)[ind], 0, sizeof(MYSQL_TIME));
      (*nul)[ind] = 0;
    }
  return 1;
}

unsigned char allocateTinyIntMemory(int ind,
				    signed char** val,
				    my_bool**     nul)
{
  if (!ind) 
    {
      (*val) = (signed char*) calloc(1,
				     sizeof(signed char));
      (*nul) = (my_bool*) calloc(1,
				 sizeof(my_bool));
    }
  else
    {
      signed char* y = (signed char *) realloc((*val),
					       (ind + 1) * sizeof(signed char));
      my_bool* w = (my_bool*) realloc((*nul),
				      (ind + 1) * sizeof(my_bool));
      if (!y || !w)
	{
	  if (y)
	    {free(y);}
	  if (w)
	    {free(w);}
	  return 0;
	}
      (*val) = y;
      (*nul) = w;
    }
  ((*val)[ind]) = (signed char) 0;
  ((*nul)[ind]) = (my_bool) 0;
  return 1;
}

unsigned char allocateShortIntMemory(int ind,
				     short int** val,
				     my_bool**   nul)
{
  if (!ind) 
    {
      (*val) = (short int*) calloc(1,
			     sizeof(short int));
      (*nul) = (my_bool*) calloc(1,
				 sizeof(my_bool));
    }
  else
    {
      short int* y = (short int*) realloc((*val),
					  (ind + 1) * sizeof(short int));
      my_bool* w = (my_bool*) realloc((*nul),
				      (ind + 1) * sizeof(my_bool));
      if (!y || !w)
	{
	  if (y)
	    {free(y);}
	  if (w)
	    {free(w);}
	  return 0;
	}
      (*val) = y;
      (*nul) = w;
    }
  ((*val)[ind]) = (short int) 0;
  ((*nul)[ind]) = (my_bool) 0;
  return 1;
}

unsigned char allocateIntMemory(int ind,
				int** val,
				my_bool**   nul)
{
  if (!ind) 
    {
      (*val) = (int*) calloc(1,
			     sizeof(int));
      (*nul) = (my_bool*) calloc(1,
				 sizeof(my_bool));
    }
  else
    {
      int* y = (int*) realloc((*val),
			      (ind + 1) * sizeof(int));
      my_bool* w = (my_bool*) realloc((*nul),
				      (ind + 1) * sizeof(my_bool));
      if (!y || !w)
	{
	  if (y)
	    {free(y);}
	  if (w)
	    {free(w);}
	  return 0;
	}
      (*val) = y;
      (*nul) = w;
    }
  ((*val)[ind]) = (int) 0;
  ((*nul)[ind]) = (my_bool) 0;
  return 1;
}

unsigned char allocateLongIntMemory(int ind,
				    long long int** val,
				    my_bool**   nul)
{
  if (!ind) 
    {
      (*val) = (long long int*) calloc(1,
				       sizeof(long long int));
      (*nul) = (my_bool*) calloc(1,
				 sizeof(my_bool));
    }
  else
    {
      long long int* y = (long long int*) realloc((*val),
						  (ind + 1) *
						  sizeof(long long int));
      my_bool* w = (my_bool*) realloc((*nul),
				      (ind + 1) * sizeof(my_bool));
      if (!y || !w)
	{
	  if (y)
	    {free(y);}
	  if (w)
	    {free(w);}
	  return 0;
	}
      (*val) = y;
      (*nul) = w;
    }
  ((*val)[ind]) = (long long int) 0;
  ((*nul)[ind]) = (my_bool) 0;
  return 1;
}

unsigned char allocateDoubleMemory(int ind,
				   double** val,
				   my_bool**   nul)
{
  if (!ind) 
    {
      (*val) = (double*) calloc(1,
				sizeof(double));
      (*nul) = (my_bool*) calloc(1,
				 sizeof(my_bool));
    }
  else
    {
      double* y = (double*) realloc((*val),
					(ind + 1) * sizeof(double));
      my_bool* w = (my_bool*) realloc((*nul),
				      (ind + 1) * sizeof(my_bool));
      if (!y || !w)
	{
	  if (y)
	    {free(y);}
	  if (w)
	    {free(w);}
	  return 0;
	}
      (*val) = y;
      (*nul) = w;
    }
  ((*val)[ind]) = (double) 0;
  ((*nul)[ind]) = (my_bool) 0;
  return 1;
}

unsigned char allocateFloatMemory(int ind,
				  float** val,
				  my_bool**   nul)
{
  if (!ind) 
    {
      (*val) = (float*) calloc(1,
			       sizeof(float));
      (*nul) = (my_bool*) calloc(1,
				 sizeof(my_bool));
    }
  else
    {
      float* y = (float*) realloc((*val),
				  (ind + 1) * sizeof(float));
      my_bool* w = (my_bool*) realloc((*nul),
				      (ind + 1) * sizeof(my_bool));
      if (!y || !w)
	{
	  if (y)
	    {free(y);}
	  if (w)
	    {free(w);}
	  return 0;
	}
      (*val) = y;
      (*nul) = w;
    }
  ((*val)[ind]) = (float) 0;
  ((*nul)[ind]) = (my_bool) 0;
  return 1;
}

void allocateMDBBindMemory(MYSQL_BIND** bindt,
			   int bindCount)
{
  (*bindt) = (MYSQL_BIND*)calloc(bindCount, sizeof(MYSQL_BIND));
}

unsigned char chMDBBindString(char* vl,
			      enum enum_field_types tp,
			      unsigned int lenVal,
	                      int* bindCount,
			      int* ind,
			      char*** val,
			      unsigned long** len,
			      enum enum_field_types** typeOrder,
			      my_bool** nul,
		              my_bool isNull)
{
  chMDBAddTypeOrderEntry(&(*typeOrder),
			 tp,
			 *bindCount);
  (*len)[(*ind)] = lenVal ? lenVal : strlen(vl);
  (*val)[(*ind)] = (char*) calloc(((*len)[(*ind)]) + 1,
				  sizeof(char));
  (*nul)[(*ind)] = isNull;
  strcpy((*val)[(*ind)++], vl);
  (*bindCount)++;
  return 1;
}

unsigned char chMDBBindTinyInt(signed char vl,
			       int* bindCount,
			       int* ind,
			       signed char** val,
			       enum enum_field_types** typeOrder,
			       my_bool** nul,
			       my_bool isNull)
{
  chMDBAddTypeOrderEntry(&(*typeOrder),
			 MYSQL_TYPE_TINY,
			 *bindCount);    
  (*val)[(*ind)] = vl;
  (*nul)[(*ind)++] = isNull;
  (*bindCount)++;
  return 1;
}

unsigned char chMDBBindShortInt(short int vl,
				int* bindCount,
				int* ind,
				short int** val,
			       enum enum_field_types** typeOrder,				
				my_bool** nul,
				my_bool isNull)
{
  chMDBAddTypeOrderEntry(&(*typeOrder),
			 MYSQL_TYPE_SHORT,
			 *bindCount);    
  (*val)[(*ind)] = vl;
  (*nul)[(*ind)++] = isNull;
  (*bindCount)++;
  return 1;
}

unsigned char chMDBBindInt(int vl,
			   int* bindCount,
			   int* ind,
			   int** val,
			   enum enum_field_types** typeOrder,
			   my_bool** nul,
			   my_bool isNull)
{
  chMDBAddTypeOrderEntry(&(*typeOrder),
			 MYSQL_TYPE_LONG,
			 *bindCount);    
  (*val)[(*ind)] = vl;
  (*nul)[(*ind)++] = isNull;
  (*bindCount)++;
  return 1;
}

unsigned char chMDBBindLongInt(long long int vl,
			       int* bindCount,
			       int* ind,
			       long long int** val,
			       enum enum_field_types** typeOrder,
			       my_bool** nul,
			       my_bool isNull)
{
  chMDBAddTypeOrderEntry(&(*typeOrder),
			 MYSQL_TYPE_LONGLONG,
			 *bindCount);  
  (*val)[(*ind)] = vl;
  (*nul)[(*ind)++] = isNull;
  (*bindCount)++;
  return 1;
}

unsigned char chMDBBindDouble(double vl,
			      int* bindCount,
			      int* ind,
			      double** val,
			      enum enum_field_types** typeOrder,			      
			      my_bool** nul,
			      my_bool isNull)
{
  chMDBAddTypeOrderEntry(&(*typeOrder),
			 MYSQL_TYPE_DOUBLE,
			 *bindCount);  
  (*val)[(*ind)] = vl;
  (*nul)[(*ind)++] = isNull;
  (*bindCount)++;
  return 1;
}

unsigned char chMDBBindFloat(float vl,
			     int* bindCount,
			     int* ind,
			     float** val,
			     enum enum_field_types** typeOrder,
			     my_bool** nul,
			     my_bool isNull)
{
  chMDBAddTypeOrderEntry(&(*typeOrder),
			 MYSQL_TYPE_FLOAT,
			 *bindCount);
  (*val)[(*ind)] = vl;
  (*nul)[(*ind)++] = isNull;
  (*bindCount)++;
  return 1;
}

unsigned char chMDBBindDateTime(unsigned int year,
				unsigned int month,
				unsigned int day,
				unsigned int hour,
				unsigned int minute,
				unsigned int second,
				int* bindCount,
				int* ind,
				MYSQL_TIME** tmL,
				enum enum_field_types** typeOrder,
				my_bool** nul,
				my_bool isNull)
{
  chMDBAddTypeOrderEntry(&(*typeOrder),
			 MYSQL_TYPE_DATETIME,
			 *bindCount);
  ((*nul)[(*ind)]) = isNull;
  ((*tmL)[(*ind)]).year = year;
  ((*tmL)[(*ind)]).month = month;
  ((*tmL)[(*ind)]).day = day;
  ((*tmL)[(*ind)]).hour = hour;
  ((*tmL)[(*ind)]).minute = minute;
  ((*tmL)[(*ind)++]).second = second;
  (*bindCount)++;
  return 1;
}


void getFieldCount()
{stmtFieldCount = mysql_stmt_field_count(stmt);}


unsigned char bindCols(char***       strVal,
		       unsigned long** strLen,
		       char**       strNul,		       
		       MYSQL_TIME** MSTime,
		       my_bool**    timeNul,
		       signed char**  tinyIntVal,
		       my_bool**    tinyIntNul,
		       short int**  shortIntVal,
		       my_bool**    shortIntNul,
		       int**        intVal,
		       my_bool**    intNul,
		       long long int**   longIntVal,
		       my_bool**    longIntNul,
		       double**     doubleVal,
		       my_bool**    doubleNul,
		       float**      floatVal,
		       my_bool**    floatNul,
		       MYSQL_BIND** b,
		       enum enum_field_types** typeOrder,
		       int bindCount)
{
  if (!bindCount)
    {return 0;}
  if ((*b))
    {
      free((*b));
      (*b) = NULL;
    }
  allocateMDBBindMemory(&(*b), bindCount);
  if (!(*b))
    {return 0;}
  int dtInd = 0,
    strInd = 0,
    tiInd = 0,
    siInd = 0,
    iInd = 0,
    liInd = 0,
    dbInd = 0,
    flInd = 0;

  for (int x = 0; x<bindCount; x++)
    {
      switch ((*typeOrder)[x])
	{
	case MYSQL_TYPE_DATETIME:
	  (*b)[x].buffer_type = (*typeOrder)[x];
	  (*b)[x].buffer = (char*) &(*MSTime)[dtInd];
	  (*b)[x].is_null = &(*timeNul)[dtInd];
	  (*b)[x].length = 0;
	  dtInd++;
	  break;
	case MYSQL_TYPE_STRING:
	case MYSQL_TYPE_VAR_STRING:
	case MYSQL_TYPE_BLOB:
	  (*b)[x].buffer_type = (*typeOrder)[x];
	  (*b)[x].buffer = (char*) (*strVal)[strInd];
	  (*b)[x].is_null = &(*strNul)[strInd];
	  (*b)[x].length = &(*strLen)[strInd];
	  (*b)[x].buffer_length = (*strLen)[strInd];
	  strInd++;
	  break;
	case MYSQL_TYPE_TINY:
	  (*b)[x].buffer_type = (*typeOrder)[x];
	  (*b)[x].buffer = (char*) &(*tinyIntVal)[tiInd];
	  (*b)[x].is_null = &(*tinyIntNul)[tiInd];
	  (*b)[x].length = 0;
	  tiInd++;
	  break;
	case MYSQL_TYPE_SHORT:
	  (*b)[x].buffer_type = (*typeOrder)[x];
	  (*b)[x].buffer = (char*) &(*shortIntVal)[siInd];
	  (*b)[x].is_null = &(*shortIntNul)[siInd];
	  (*b)[x].length = 0;
	  siInd++;
	  break;
	case MYSQL_TYPE_INT24:
	case MYSQL_TYPE_LONG:
	  (*b)[x].buffer_type = (*typeOrder)[x];
	  (*b)[x].buffer = (char*) &(*intVal)[iInd];
	  (*b)[x].is_null = &(*intNul)[iInd];
	  (*b)[x].length = 0;
	  iInd++;
	  break;
	case MYSQL_TYPE_LONGLONG:
	  (*b)[x].buffer_type = (*typeOrder)[x];
	  (*b)[x].buffer = (char*) &(*longIntVal)[liInd];
	  (*b)[x].is_null = &(*longIntNul)[liInd];
	  (*b)[x].length = 0;
	  liInd++;
	  break;
	case MYSQL_TYPE_DOUBLE:
	  (*b)[x].buffer_type = (*typeOrder)[x];
	  (*b)[x].buffer = (char*) &(*doubleVal)[dbInd];
	  (*b)[x].is_null = &(*doubleNul)[dbInd];
	  (*b)[x].length = 0;
	  dbInd++;
	  break;
	case MYSQL_TYPE_FLOAT:
	  (*b)[x].buffer_type = (*typeOrder)[x];
	  (*b)[x].buffer = (char*) &(*floatVal)[flInd];
	  (*b)[x].is_null = &(*floatNul)[flInd];
	  (*b)[x].length = 0;
	  flInd++;
	  break;
	default:
	  break;
	  	  
	}
    }
}


unsigned char prepareResultCols()
{
  if (!stmtFieldCount)
    {return 0;}
  chMDBClearDateTimeMemory(&resultColDateTimeInd,
			   &resultColMSTime,
			   &resultColDateTimeNull);
  chMDBClearStringMemory(&resultColStringInd,
			 &resultColStringVal,
			 &resultColStringLen,
			 &resultColStringNull);
  chMDBClearTinyIntMemory(&resultColTinyIntInd,
			  &resultColTinyIntVal,
			  &resultColTinyIntNull);
  chMDBClearShortIntMemory(&resultColShortIntInd,
			   &resultColShortIntVal,
			   &resultColShortIntNull);
  chMDBClearIntMemory(&resultColIntInd,
		      &resultColIntVal,
		      &resultColIntNull);
  chMDBClearLongIntMemory(&resultColLongIntInd,
			  &resultColLongIntVal,
			  &resultColLongIntNull);
  chMDBClearDoubleMemory(&resultColDoubleInd,
			 &resultColDoubleVal,
			 &resultColDoubleNull);
  chMDBClearFloatMemory(&resultColFloatInd,
			&resultColFloatVal,
			&resultColFloatNull);
  if (resMetaData)
    {
      mysql_free_result(resMetaData);
      resMetaData = NULL;
      resFld = NULL;
    }
  resMetaData = mysql_stmt_result_metadata(stmt);
  resFld = mysql_fetch_fields(resMetaData);
  resBnd = NULL;
  resBindCount = 0;
  for (int x = 0; x<stmtFieldCount; x++)
    {
      switch (resFld[x].type)
	{
	case MYSQL_TYPE_STRING:
	case MYSQL_TYPE_VAR_STRING:
	case MYSQL_TYPE_BLOB:
	  if (!allocateStringMemory(resultColStringInd,
				    &resultColStringVal,
				    &resultColStringLen,
				    &resultColStringNull))
	    {return 0;}
	  chMDBBindString("",
			  resFld[x].type,
			  resFld[x].length,
			  &resBindCount,
			  &resultColStringInd,
			  &resultColStringVal,
			  &resultColStringLen,
			  &resTypeOrder,
			  &resultColStringNull,
			  0);
	  break;
	case MYSQL_TYPE_TINY:
	  if (!allocateTinyIntMemory(resultColTinyIntInd,
				     &resultColTinyIntVal,
				     &resultColTinyIntNull))
	    {return 0;}
	  chMDBBindTinyInt(0,
	  		   &resBindCount,
	  		   &resultColTinyIntInd,
	  		   &resultColTinyIntVal,
			   &resTypeOrder,
	  		   &resultColTinyIntNull,
	  		   0);
	  break;
	case MYSQL_TYPE_SHORT:
	  if (!allocateShortIntMemory(resultColShortIntInd,
				      &resultColShortIntVal,
				      &resultColShortIntNull))
	    {return 0;}
	  chMDBBindShortInt(0,
			    &resBindCount,
			    &resultColShortIntInd,
			    &resultColShortIntVal,
			    &resTypeOrder,
			    &resultColShortIntNull,
	  		    0);
	  break;
	case MYSQL_TYPE_INT24:
	case MYSQL_TYPE_LONG:
	  if (!allocateIntMemory(resultColIntInd,
				 &resultColIntVal,
				 &resultColIntNull))
	    {return 0;}
	  chMDBBindInt(0,
		       &resBindCount,
		       &resultColIntInd,
		       &resultColIntVal,
		       &resTypeOrder,
		       &resultColIntNull,
		       0);
	  break;
	case MYSQL_TYPE_LONGLONG:
	  if (!allocateLongIntMemory(resultColLongIntInd,
				     &resultColLongIntVal,
				     &resultColLongIntNull))
	    {return 0;}
	  chMDBBindLongInt(0,
			   &resBindCount,
			   &resultColLongIntInd,
			   &resultColLongIntVal,
			   &resTypeOrder,
			   &resultColLongIntNull,
			   0);
	  break;	  
	case MYSQL_TYPE_DOUBLE:
	  if (!allocateDoubleMemory(resultColDoubleInd,
				    &resultColDoubleVal,
				    &resultColDoubleNull))
	    {return 0;}
	  chMDBBindDouble(0,
			  &resBindCount,
			  &resultColDoubleInd,
			  &resultColDoubleVal,
			  &resTypeOrder,
			  &resultColDoubleNull,
			  0);
	  break;
	case MYSQL_TYPE_FLOAT:
	  if (!allocateFloatMemory(resultColFloatInd,
				   &resultColFloatVal,
				   &resultColFloatNull))
	    {return 0;}
	  chMDBBindFloat(0,
			 &resBindCount,
			 &resultColFloatInd,
			 &resultColFloatVal,
			 &resTypeOrder,
			 &resultColFloatNull,
			 0);
	  break;
	case MYSQL_TYPE_DATETIME:
	  if (!allocateDateTimeMemory(resultColDateTimeInd,
				      &resultColMSTime,
				      &resultColDateTimeNull))
	    {return 0;}
	  chMDBBindDateTime(0,
			    0,
			    0,
			    0,
			    0,
			    0,
			    &resBindCount,
			    &resultColDateTimeInd,
			    &resultColMSTime,
			    &resTypeOrder,
			    &resultColDateTimeNull,
			    0);	  
	  break;
	default:
	  continue;
	}
    }
  return 1;
}

unsigned int getRow()
{
  resStat = mysql_stmt_fetch(stmt);
  if (resStat == 1 || resStat == MYSQL_NO_DATA)
    {return 0;}
  resRowCount++;
  return 1;
}

char* getStringFieldValue(int i)
{return resultColStringVal[i];}

signed char getTinyIntFieldValue(int i)
{return resultColTinyIntVal[i];}

short int getShortIntFieldValue(int i)
{return resultColShortIntVal[i];}

int getIntFieldValue(int i)
{return resultColIntVal[i];}

long long int getLongIntFieldValue(int i)
{return resultColLongIntVal[i];}

double getDoubleFieldValue(int i)
{return resultColDoubleVal[i];}

float getFloatFieldValue(int i)
{return resultColFloatVal[i];}

unsigned int getDateTimeFieldYear(int i)
{return resultColMSTime[i].year;}

unsigned int getDateTimeFieldMonth(int i)
{return resultColMSTime[i].month;}

unsigned int getDateTimeFieldDay(int i)
{return resultColMSTime[i].day;}

unsigned int getDateTimeFieldHour(int i)
{return resultColMSTime[i].hour;}

unsigned int getDateTimeFieldMinute(int i)
{return resultColMSTime[i].minute;}

unsigned int getDateTimeFieldSecond(int i)
{return resultColMSTime[i].second;}
