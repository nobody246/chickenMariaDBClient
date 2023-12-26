# chickenMariaDBClient
MySQL/Maria DB connector binding for Chicken Scheme

Released under LGPL v2 or MIT (your choice of either)

I'm not responsible for anything.

# Build:

csc -s chicken-mariadb-client.scm -emit-import-library chicken-mariadb-client -lmysqlclient 


# Select:

#;1> (use chicken-mariadb-client)

#;2> (connect "user" "password" "db")

1

#;3> (prepare "select * from table")

1

#;4> (execute)

1

#;5> (get-row-values)

(267 #f #f #f #f #f #f #f #f #f #f #f #f #f #f #f #f #f (0 0 0 0 0 0) (1998 8 8 8 8 8) (1833 3 3 3 3 3))



# Bind:

#;1> (use chicken-mariadb-client)

#;2> (connect "user" "password" "db")

1

#;3> (prepare "insert into table (v1, v2, v3) values (?, ?, ?)") ; assume all three values are ints

1

#;4> (bind-int 111 #f) (bind-int 222 #f) (bind-int 333 #f)

1

1

1

#;5) (execute)

1


# More documentation coming soon!





