#ifndef HEAD_H_
#define HEAD_H_

#include <stdio.h>

#define ATTR_LOCKED 0
#define ATTR_UNSET 1
#define ATTR_UNSHOW 2
#define LOCKNUM 3
#define MAXSTR 31
#define MAXATTR 10

/*属性结构*/
typedef char** Attr;

/*学生数据库结构（链表）*/
struct StuDataBase
{
	char *id;   /*默认属性：学号*/
    char *name;     /*默认属性：姓名*/
    Attr attr;      /*属性*/
    struct StuDataBase *next;   /*链表指针*/
};
typedef struct StuDataBase StuDataBase;
typedef StuDataBase *Stu;

/*配置文件结构*/
typedef struct Config
{
    char fileName[MAXSTR*3];
    char attrList[MAXATTR][MAXSTR];
    int attrNum;
    int lockedAttr[LOCKNUM][MAXATTR];
    long stuNum;
}Config;

/*--------------函数声明--------------*/

/*配置文件操作*/
void readConfig(Config *config);    /*读配置*/
void saveConfig(Config *config);    /*保存配置*/
void syncConfig(char *fileName,Config *config); /*同步配置与学生数据（同步学生人数信息）*/
void setConfig(char *fileName, Config *config); /*设置配置（更改属性锁）*/

/*管理学生数据库*/
Stu stuDataBaseInit(Stu stu, Config *config);   /*为某一节点分配内存（链表操作）*/
void readFromFile(char *filename, Stu stu, Config *config); /*从文件中读取配置*/
void writeToFile(char *filename, Stu stu, Config *config);  /*写入数据库*/
void readFromKey(char *filename, Stu stu, Config *config);  /*从键盘读入学生信息*/
Stu delStu(char *attrName, char *info, Stu stu, Config *config);   /*从数据库中删除学生（链表操作）*/
Stu findPrevious(Stu head, Stu stu);    /*寻找前一节点（链表操作）*/
void disposeInfo(Stu stu, Config *config);  /*释放空间*/

void addAttr(char *attrName, Stu stu, Config *config);  /*添加属性*/
void fillAttr(char *attrName, Stu stu, Config *config); /*填充属性*/
/*用新属性替换某一属性*/
void replaceAttr(char *attrName,char *newAttrName, char *filename, Stu stu, Config *config);
int indexOfAttr(char *attrName, Config *config);    /*获取某一属性对应下标*/

/*随机点名函数*/
void rollcall(Stu stu, char *attrName, char *keyword, Config *config);

/*显示列表*/
void showMenu();    /*打印菜单*/
void showAttr(Config *config);  /*打印属性列表及属性状态*/
void showData(Stu stu, Config *config); /*打印学生信息*/

/*按学号排序函数*/
void quickSort(Stu pBegin, Stu pEnd);   /*快速排序*/
Stu getPartion(Stu pBegin, Stu pEnd);   /*快排获取分割点*/
void swapStu(Stu stu1, Stu stu2);   /*交换学生信息*/

/*自定义拓展标准函数*/
char *myfgets(char *buff, int n, FILE *fp);
char *mystrncpy(char *dest, char *src, size_t n, int start);
#endif
