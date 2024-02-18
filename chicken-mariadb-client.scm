                                        ;Chicken Scheme MariaDB Connector
                                        ;(c) 12-25-2023 github.com/nobody246 Alex S. a.se8seven(a)gm
                                        ;Released under LGPL v2 or MIT (your choice of either)
                                        ;I'm not responsible for anything.

[module
  chicken-mariadb-client
  (connect-server connect prepare bind-string-type bind-string
   bind-datetime bind-tiny-int bind-short-int bind-int bind-long-int
   bind-double bind-float execute get-field-count get-row get-string-value
   get-tiny-int-value get-short-int-value get-int-value get-long-int-value
   get-float-value get-double-value get-datetime-value null-check-datetime
   null-check-string null-check-tiny-int null-check-short-int null-check-int
   null-check-long-int null-check-float null-check-double get-col-datatype
   get-result-status get-row-values mdb-error mdb-errno close-connection $n
   datatypes)
  (import scheme chicken foreign data-structures)
  
  (foreign-declare "#include \"chicken_mariadb.h\"")
  
  (define-syntax $n
    (syntax-rules ()
      (($n body)
       (foreign-value body unsigned-integer))))

  (define datatypes
    `(,(cons 'tiny-int     ($n MYSQL_TYPE_TINY))
      ,(cons 'short-int    ($n MYSQL_TYPE_SHORT))
      ,(cons 'int          ($n MYSQL_TYPE_LONG))
      ,(cons 'long-int     ($n MYSQL_TYPE_LONGLONG))
      ,(cons 'int-24       ($n MYSQL_TYPE_INT24))
      ,(cons 'double       ($n MYSQL_TYPE_DOUBLE))
      ,(cons 'float        ($n MYSQL_TYPE_FLOAT))
      ,(cons 'datetime     ($n MYSQL_TYPE_DATETIME))
      ,(cons 'string       ($n MYSQL_TYPE_STRING))
      ,(cons 'var-string   ($n MYSQL_TYPE_VAR_STRING))
      ,(cons 'blob         ($n MYSQL_TYPE_BLOB))))

  ;not tested
  (define connect-server
    (foreign-lambda* number
        ((c-string server)
         (c-string username)
         (c-string password)
         (c-string database)
         (unsigned-byte port)
         (c-string unixsocket)
         (unsigned-long clientflag))
      "con = mysql_init(NULL);
       if (!con)
        {C_return(0);}
       if (!(mysql_real_connect(con,
                               server,
                               username,
                               password,
                               database,
                               port,
                               unixsocket,
                               clientflag)))
        {C_return(0);}
       C_return(1);"))

  (define connect
    (foreign-lambda* number
        ((c-string username)
         (c-string password)
         (c-string database))
      "con = mysql_init(NULL);
       if (!con)
        {C_return(0);}
       if (!(mysql_real_connect(con,
                               \"localhost\",
                               username,
                               password,
                               database,
                               0,
                               NULL,
                               0)))
        {C_return(0);}
       C_return(1);"))

  (define prepare
    (foreign-lambda* number ((c-string sql))
      "if (!con || !sql)
        {C_return(0);}
       cleanup();
       sqlStr = (char*) calloc((strlen(sql) + 1), sizeof(char));
       strcpy(sqlStr, sql);
       stmt = mysql_stmt_init(con);
       if (!stmt)
        {C_return(0);}
       if (mysql_stmt_prepare(stmt, sqlStr, strlen(sqlStr)))
        {C_return(0);}
       C_return(1);"))

  
  (define bind-string-type
    (foreign-lambda* number
        ((c-string vl)
         (unsigned-byte tp)
         (bool nul))
      "if (!con || !stmt)
        {C_return(0);}
       if (!allocateStringMemory(paramStringInd,
                                 &paramStringVal,
                                 &paramStringLen,
                                 &paramStringNull))
        {C_return(0);}
       C_return((int)chMDBBindString(vl,
                                     tp,
                                     0,
                                     &bindCount,
                                     &paramStringInd,
                                     &paramStringVal,
                                     &paramStringLen,
                                     &paramTypeOrder,
                                     &paramStringNull,
                                     nul));"))

  (define (bind-string vl nul)
    (when (string? vl)
      (bind-string-type vl
                        (alist-ref
                         'string
                         datatypes)
                        nul)))
    
  
  (define bind-datetime
    (foreign-lambda* number
        ((unsigned-int year)
         (unsigned-int month)
         (unsigned-int day)
         (unsigned-int hour)
         (unsigned-int minute)
         (unsigned-int second)
         (bool nul))
      "if (!con || !stmt)
        {C_return(0);}
       if (!allocateDateTimeMemory(paramDateTimeInd,
                                   &paramMSTime,
                                   &paramDateTimeNull))
        {return 0;}
       C_return((int) chMDBBindDateTime(year,
                                        month,
                                        day,
                                        hour,
                                        minute,
                                        second,
                                        &bindCount,
                                        &paramDateTimeInd,
                                        &paramMSTime,
                                        &paramTypeOrder,
                                        &paramDateTimeNull,
                                        nul));"))

  (define bind-tiny-int
      (foreign-lambda* number
          ((byte vl)
           (bool nul))
        "if (!con || !stmt)
          {C_return(0);}
         if (!allocateTinyIntMemory(paramTinyIntInd,
                                    &paramTinyIntVal,
                                    &paramTinyIntNull))
          {C_return(0);}
         C_return((int)chMDBBindTinyInt(vl,
                                        &bindCount,
                                        &paramTinyIntInd,
                                        &paramTinyIntVal,
                                        &paramTypeOrder,
                                        &paramTinyIntNull,
                                        nul));"))

  (define bind-short-int
      (foreign-lambda* number
          ((short vl)
           (bool nul))
        "if (!con || !stmt)
          {C_return(0);}
         if (!allocateShortIntMemory(paramShortIntInd,
                                     &paramShortIntVal,
                                     &paramShortIntNull))
          {C_return(0);}
         C_return((int)chMDBBindShortInt(vl,
                                         &bindCount,
                                         &paramShortIntInd,
                                         &paramShortIntVal,
                                         &paramTypeOrder,
                                         &paramShortIntNull,
                                         nul));"))

  (define bind-int
      (foreign-lambda* number
          ((integer vl)
           (bool nul))
        "if (!con || !stmt)
          {C_return(0);}
         if (!allocateIntMemory(paramIntInd,
                                &paramIntVal,
                                &paramIntNull))
          {C_return(0);}
         C_return((int)chMDBBindInt(vl,
                                    &bindCount,
                                    &paramIntInd,
                                    &paramIntVal,
                                    &paramTypeOrder,
                                    &paramIntNull,
                                    nul));"))

  (define bind-long-int
      (foreign-lambda* number
          ((long vl)
           (bool nul))
        "if (!con || !stmt)
          {C_return(0);}
         if (!allocateLongIntMemory(paramLongIntInd,
                                    &paramLongIntVal,
                                    &paramLongIntNull))
          {C_return(0);}
         C_return((int)chMDBBindLongInt(vl,
                                        &bindCount,
                                        &paramLongIntInd,
                                        &paramLongIntVal,
                                        &paramTypeOrder,
                                        &paramLongIntNull,
                                        nul));"))

  (define bind-double
      (foreign-lambda* number
          ((double vl)
           (bool nul))
        "if (!con || !stmt)
          {C_return(0);}
         if (!allocateDoubleMemory(paramDoubleInd,
                                   &paramDoubleVal,
                                   &paramDoubleNull))
          {C_return(0);}
         C_return((int)chMDBBindDouble(vl,
                                       &bindCount,
                                       &paramDoubleInd,
                                       &paramDoubleVal,
                                       &paramTypeOrder,
                                       &paramDoubleNull,
                                       nul));"))

  (define bind-float
      (foreign-lambda* number
          ((float vl)
           (bool nul))
        "if (!con || !stmt)
          {C_return(0);}
         if (!allocateFloatMemory(paramFloatInd,
                                  &paramFloatVal,
                                  &paramFloatNull))
          {C_return(0);}
         C_return(chMDBBindFloat(vl,
                                 &bindCount,
                                 &paramFloatInd,
                                 &paramFloatVal,
                                 &paramTypeOrder,
                                 &paramFloatNull,
                                 nul));"))


  (define execute
      (foreign-lambda* number
          ()
        "if (!con || !stmt)
         {C_return(0);}
        if (bindCount)
         {
            bindCols(&paramStringVal,
                     &paramStringLen,
                     &paramStringNull,
                     &paramMSTime,
                     &paramDateTimeNull,
                     &paramTinyIntVal,
                     &paramTinyIntNull,
                     &paramShortIntVal,
                     &paramShortIntNull,
                     &paramIntVal,
                     &paramIntNull,
                     &paramLongIntVal,
                     &paramLongIntNull,
                     &paramDoubleVal,
                     &paramDoubleNull,
                     &paramFloatVal,
                     &paramFloatNull,
                     &bnd,
                     &paramTypeOrder,
                     bindCount);
         }
        if (bnd)
         {
           if (mysql_stmt_bind_param(stmt, bnd))
            {C_return(0);}
         }
        getFieldCount();
        if (mysql_stmt_execute(stmt))
         {C_return(0);}
        if (stmtFieldCount)
         {
           prepareResultCols();
           bindCols(&resultColStringVal,
                    &resultColStringLen,
                    &resultColStringNull,
                    &resultColMSTime,
                    &resultColDateTimeNull,
                    &resultColTinyIntVal,
                    &resultColTinyIntNull,
                    &resultColShortIntVal,
                    &resultColShortIntNull,
                    &resultColIntVal,
                    &resultColIntNull,
                    &resultColLongIntVal,
                    &resultColLongIntNull,
                    &resultColDoubleVal,
                    &resultColDoubleNull,
                    &resultColFloatVal,
                    &resultColFloatNull,
                    &resBnd,
                    &resTypeOrder,
                    resBindCount);
           mysql_stmt_bind_result(stmt, resBnd);
           if(mysql_stmt_store_result(stmt))
             {C_return(0);} 
         }
        C_return(1);"))

    (define get-field-count
      (foreign-lambda* number
          ()
        "if (!con || !stmt)
          {C_return(0);}
         getFieldCount();
         C_return(stmtFieldCount);"))

    (define get-row
      (foreign-lambda* number
          ()
        "if (!con || !stmt || !stmtFieldCount)
          {C_return(0);}
         C_return(getRow());"))

    (define get-string-value
      (foreign-lambda* c-string
          ((unsigned-integer index))
        "if (!resultColStringInd ||
             index > (resultColStringInd - 1))
          {C_return(0);}
         C_return(getStringFieldValue(index));"))

    (define get-tiny-int-value
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColTinyIntInd ||
             index > (resultColTinyIntInd - 1))
          {C_return(0);}
         C_return(getTinyIntFieldValue(index));"))

    (define get-short-int-value
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColShortIntInd ||
             index > (resultColShortIntInd - 1))
          {C_return(0);}
         C_return(getShortIntFieldValue(index));"))

    (define get-int-value
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColIntInd ||
             index > (resultColIntInd - 1))
          {C_return(0);}
         C_return(getIntFieldValue(index));"))    

    (define get-long-int-value
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColLongIntInd ||
             index > (resultColLongIntInd - 1))
          {C_return(0);}
         C_return(getLongIntFieldValue(index));"))    

    (define get-double-value
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColDoubleInd ||
             index > (resultColDoubleInd - 1))
          {C_return(0);}
         C_return(getDoubleFieldValue(index));"))

    
    (define get-float-value
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColFloatInd ||
             index > (resultColFloatInd - 1))
          {C_return(0);}
         C_return(getFloatFieldValue(index));"))

    (define get-datetime-year
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColDateTimeInd ||
             index > (resultColDateTimeInd - 1))
          {C_return(0);}
         C_return(getDateTimeFieldYear(index));"))

    (define get-datetime-month
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColDateTimeInd ||
             index > (resultColDateTimeInd - 1))
          {C_return(0);}
         C_return(getDateTimeFieldMonth(index));"))

    (define get-datetime-day
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColDateTimeInd ||
             index > (resultColDateTimeInd - 1))
          {C_return(0);}
         C_return(getDateTimeFieldDay(index));"))    

    (define get-datetime-hour
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColDateTimeInd ||
             index > (resultColDateTimeInd - 1))
          {C_return(0);}
         C_return(getDateTimeFieldHour(index));"))

    (define get-datetime-minute
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColDateTimeInd ||
             index > (resultColDateTimeInd - 1))
          {C_return(0);}
         C_return(getDateTimeFieldMinute(index));"))

    (define get-datetime-second
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColDateTimeInd ||
             index > (resultColDateTimeInd - 1))
          {C_return(0);}
         C_return(getDateTimeFieldSecond(index));"))

    (define (get-datetime-value index)
      `(,(get-datetime-year index)
        ,(get-datetime-month index)
        ,(get-datetime-day index)
        ,(get-datetime-hour index)
        ,(get-datetime-minute index)
        ,(get-datetime-second index)))
    
    (define null-check-string
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColStringInd ||
             index > (resultColStringInd - 1))
          {C_return(0);}
         C_return(resultColStringNull[index]);"))

    (define null-check-datetime
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColDateTimeInd ||
             index > (resultColDateTimeInd - 1))
          {C_return(0);}
         C_return(resultColDateTimeNull[index]);"))
        
    (define null-check-tiny-int
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColTinyIntInd ||
             index > (resultColTinyIntInd - 1))
          {C_return(0);}
         C_return(resultColTinyIntNull[index]);"))

    (define null-check-short-int
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColShortIntInd ||
             index > (resultColShortIntInd - 1))
          {C_return(0);}
         C_return(resultColShortIntNull[index]);"))

    (define null-check-int
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColIntInd ||
             index > (resultColIntInd - 1))
          {C_return(0);}
         C_return(resultColIntNull[index]);"))

    (define null-check-long-int
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColLongIntInd ||
             index > (resultColLongIntInd - 1))
          {C_return(0);}
         C_return(resultColLongIntNull[index]);"))

    
    (define null-check-double
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!resultColDoubleInd ||
             index > (resultColDoubleInd - 1))
          {C_return(0);}
         C_return(resultColDoubleNull[index]);"))

    (define null-check-float
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (index > (resultColFloatInd - 1) ||
             !resultColFloatInd)
          {C_return(0);}
         C_return(resultColFloatNull[index]);"))

    (define get-col-datatype
      (foreign-lambda* number
          ((unsigned-integer index))
        "if (!stmtFieldCount ||
             index > (stmtFieldCount - 1))
          {C_return(0);}
         C_return(resTypeOrder[index]);"))

    (define get-result-status
      (foreign-lambda* number
          ()
        "C_return(resStat);"))
    
    (define (get-row-values)
      (let ((x (get-field-count))
            (datetime-ind 0)
            (string-ind 0)
            (tiny-int-ind 0)
            (short-int-ind 0)
            (int-ind 0)
            (long-int-ind 0)
            (double-ind 0)
            (float-ind 0)
            (r (get-row))
            (result-status (get-result-status))
            (result '()))
        (if (or
             (= r 0)
             (and (= r 1)
                  (or (= result-status 1)
                      (= result-status ($n MYSQL_NO_DATA)))))
            #f
            (begin
        (define (iter i)
          (when (< i x)
            (cond  
             ((= (get-col-datatype i) (alist-ref 'datetime datatypes))
              (set! result
                (append result                 
                        (if (= (null-check-datetime datetime-ind) 0)
                            `(,(get-datetime-value datetime-ind))
                            '(#f))))
                    (set! datetime-ind (add1 datetime-ind)))
             ((or
               (= (get-col-datatype i) (alist-ref 'blob datatypes))
               (= (get-col-datatype i) (alist-ref 'string datatypes))
               (= (get-col-datatype i) (alist-ref 'var-string datatypes)))
              (set! result
                (append result
                        (if (= (null-check-string string-ind) 0)
                            `(,(get-string-value string-ind))
                            '(#f))))
              (set! string-ind (add1 string-ind)))
             ((= (get-col-datatype i) (alist-ref 'tiny-int datatypes))
              (set! result
                (append result                 
                        (if (= (null-check-tiny-int tiny-int-ind) 0)
                            `(,(get-tiny-int-value tiny-int-ind))
                            '(#f))))
              (set! tiny-int-ind (add1 tiny-int-ind)))
             ((= (get-col-datatype i) (alist-ref 'short-int datatypes))
              (set! result
                (append result                 
                        (if (= (null-check-short-int short-int-ind) 0)
                            `(,(get-short-int-value short-int-ind))
                            '(#f))))
              (set! short-int-ind (add1 short-int-ind)))
             ((or (= (get-col-datatype i) (alist-ref 'int-24 datatypes))
                  (= (get-col-datatype i) (alist-ref 'int datatypes)))
              (set! result
                (append result                 
                        (if (= (null-check-int int-ind) 0)
                            `(,(get-int-value int-ind))
                            '(#f))))
              (set! int-ind (add1 int-ind)))
             ((= (get-col-datatype i) (alist-ref 'long-int datatypes))
              (set! result
                (append result                 
                        (if (= (null-check-long-int long-int-ind) 0)
                            `(,(get-long-int-value long-int-ind))
                            '(#f))))
              (set! long-int-ind (add1 long-int-ind)))
             ((= (get-col-datatype i) (alist-ref 'double datatypes))
              (set! result
                (append result                 
                        (if (= (null-check-double double-ind) 0)
                            `(,(get-double-value double-ind))
                            '(#f))))
              (set! double-ind (add1 double-ind)))
             ((= (get-col-datatype i) (alist-ref 'float datatypes))
              (set! result
                (append result                 
                        (if (= (null-check-double float-ind) 0)
                            `(,(get-float-value float-ind))
                            '(#f))))
              (set! float-ind (add1 float-ind))))
          (iter (add1 i))))
        (iter 0)
        result))))
                    
    (define mdb-error
      (foreign-lambda* c-string
          ()
        "if (!con)
          {C_return(\"\");}
         C_return(mysql_error(con));"))

    (define mdb-errno
      (foreign-lambda* integer
          ()
        "if (!con)
          {C_return(0);}
         C_return(mysql_errno(con));"))    
     
    (define close-connection
      (foreign-lambda* number
          ()
        "if (!con)
          {C_return(0);}
         cleanup();
         C_return(1);"))]

    
    
    
  
 
