OBJS=mm.o odt_connect.o odt_event.o odt_rbtree.o odt_task.o \
rbtree.o odt_log.o odt_time.o odt_string.o odt_session.o    \
odt_timer.o odt_csv_parser.o odt_csv_fetcher.o odt_rtsp.o   \
main.o
odt : $(OBJS)
	gcc -o odt $(OBJS) -lrt


mm.o : odt.h mm.c
	gcc -g -c mm.c -o mm.o
rbtree.o : rbtree.h rbtree.c
	gcc -g -c rbtree.c -o rbtree.o
odt_rbtree.o : odt_rbtree.h odt_rbtree.c
	gcc -g -c odt_rbtree.c -o odt_rbtree.o
odt_connect.o : odt.h odt_connect.h odt_connect.c
	gcc -g -c odt_connect.c -o odt_connect.o
odt_event.o : odt.h odt_event.h odt_event.c
	gcc -g -c odt_event.c -o odt_event.o
odt_rtsp.o : odt.h odt_protocol_parser.h odt_rtsp.c list.h
	gcc -g -c odt_rtsp.c -o odt_rtsp.o
odt_log.o : odt.h odt_log.h odt_log.c
	gcc -g -c odt_log.c -o odt_log.o
odt_time.o : odt.h odt_time.c
	gcc -g -c odt_time.c -o odt_time.o
odt_string.o: odt.h odt_string.h odt_string.c
	gcc -g -c odt_string.c -o odt_string.o
odt_csv_parser.o : odt.h odt_csv_parser.c odt_csv.h
	gcc -g -c odt_csv_parser.c -o odt_csv_parser.o
odt_csv_fetcher.o : odt.h odt_csv_fetcher.c odt_csv.h
	gcc -g -c odt_csv_fetcher.c -o odt_csv_fetcher.o
odt_task.o : odt.h odt_task.c odt_task.h list.h
	gcc -g -c odt_task.c -o odt_task.o
odt_session.o : odt.h odt_session.h odt_session.c
	gcc -g -c odt_session.c -o odt_session.o
odt_timer.o : odt.h odt_timer.c
	gcc -g -c odt_timer.c -o odt_timer.o
main.o : odt.h main.c
	gcc -g -c main.c -o main.o


LUA_INC=-I/home/lua-5.0/include/
LUA_LIB=-L/home/lua-5.0/lib




###############################################################
#                   Test the rbtree
test_rbtree.o : rbtree.h odt_rbtree.h test_rbtree.c
	gcc -g -c test_rbtree.c -o test_rbtree.o

RBTREE_OBJS=mm.o rbtree.o odt_rbtree.o test_rbtree.o
rbtree : $(RBTREE_OBJS)
	gcc -g -o rbtree $(RBTREE_OBJS)
###############################################################



###############################################################
#                   Test the rtsp parser
test_rtsp.o : odt.h test_rtsp.c
	gcc -g -c test_rtsp.c -o test_rtsp.o

RTSP_OBJS=mm.o rbtree.o odt_rbtree.o odt_connect.o odt_event.o\
odt_rtsp.o odt_log.o odt_string.o odt_time.o test_rtsp.o
rtsp : $(RTSP_OBJS)
	gcc -g -o rtsp $(RTSP_OBJS) -lrt
###############################################################


###############################################################
#                   Test the csv
test_csv.o : odt.h test_csv.c
	gcc -g -c test_csv.c -o test_csv.o

CSV_OBJS=mm.o rbtree.o odt_rbtree.o odt_connect.o odt_event.o\
odt_rtsp.o odt_log.o odt_string.o odt_time.o odt_csv_parser.o\
test_csv.o odt_csv_fetcher.o

csv : $(CSV_OBJS)
	gcc -g -o csv $(CSV_OBJS) -lrt
###############################################################



###############################################################
#                   Test the task
test_task.o : odt.h test_task.c
	gcc -g -c test_task.c -o test_task.o

TASK_OBJS=mm.o rbtree.o odt_rbtree.o odt_connect.o odt_event.o\
odt_rtsp.o odt_log.o odt_string.o odt_time.o odt_csv_parser.o\
odt_csv_fetcher.o odt_task.o test_task.o

task : $(TASK_OBJS)
	gcc -g -o task $(TASK_OBJS) -lrt
###############################################################



###############################################################
#                   Test the lua adapter
test_lua.o : odt.h test_lua.c odt_lua_adapter.h
	gcc -g -c test_lua.c -o test_lua.o 

LUA_OBJS=mm.o rbtree.o odt_rbtree.o odt_connect.o odt_event.o\
odt_rtsp.o odt_log.o odt_string.o odt_time.o odt_csv_parser.o\
odt_csv_fetcher.o odt_task.o test_lua.o main.o odt_session.o\
odt_timer.o

lua : $(LUA_OBJS)
	gcc -g -o lua $(LUA_OBJS) -lrt $(LUA_LIB) -llua $(LUA_INC)\
       -llualib 
###############################################################



###############################################################
#                   Test the log
test_log.o : odt.h test_log.c odt_log.h
	gcc -g -c test_log.c -o test_log.o

LOG_OBJS=odt_log.o test_log.o odt_time.o odt_string.o mm.o

log : $(LOG_OBJS)
	gcc -g -o llog $(LOG_OBJS) -lrt
###############################################################






.PHONY : clean
clean :
	rm -f *.o
	rm -f rtsp
	rm -f a.out
	rm -f odtool
	rm -f csv
	rm -f rbtree
	rm -f task
	rm -f llog
	rm -f lua
cleanlog :
	rm -f *.o
	rm -f rtsp
	rm -f a.out
	rm -f odtool
	rm -f csv
	rm -f rbtree
	rm -f task
	rm -f llog
	rm -f lua
	rm -fr log
