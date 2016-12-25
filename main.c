/*2016-12-23*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "head.h"

int main()
{
    Stu stu = (Stu)malloc(sizeof(StuDataBase));
    Stu head = stu;
    Stu stuTemp=stu;
    Config configure;
    Config *config = &configure;
    /*定义默认文件名*/
    strncpy(config->fileName, "/home/xy16/CCode.d/RollCall2/config", MAXSTR*2);
    char fileName[] = "/home/xy16/CCode.d/RollCall2/stuDataBase";

    char attrName[MAXSTR];
    char newAttrName[MAXSTR];
    char info[MAXSTR];

    int stuNum;

    int select;
    int i;

    char *myfgetsReturn;

    /*初始化并读入配置文件*/
    readConfig(config);
    syncConfig(fileName,config);

    /*首先获取学生数目和属性数量信息*/
    stuNum = config->stuNum;

    /*初始化学生数据库*/
    for(i=0;i<stuNum;i++)
    {
       stuTemp = stuDataBaseInit(stuTemp, config);
    }
    readFromFile(fileName, stu, config);

    do
    {
        stu = head;
        showMenu();
        scanf("%d", &select);
        getchar();
        switch(select)
        {
            case 0:
                printf("谢谢使用\n");
                exit(0);
            /*按条件点名*/
            case 1:
                printf("请输入要查询的属性名(默认为ID)：");
                myfgetsReturn = myfgets(attrName, MAXSTR-1, stdin);
                if(myfgetsReturn!=NULL && !strcmp(myfgetsReturn, ""))
                    strcpy(attrName,"ID");
                printf("请输入需要查找的关键字(默认为空)：");
                myfgets(info, MAXSTR-1, stdin);
                rollcall(stu, attrName, info, config);
                break;
            /*从键盘读入学生信息*/
            case 2:
                readFromKey(fileName, stu, config);
                break;
            /*显示文件信息*/
            case 3:
                showData(stu, config);
                break;
            /*进行设置*/
            case 4:
                setConfig(fileName, config);
                break;
            /*显示属性*/
            case 5:
                showAttr(config);
                break;
            /*添加属性*/
            case 6:
                printf("请输入要添加的属性名：");
                myfgets(attrName, MAXSTR, stdin);
                addAttr(attrName, stu, config);
                printf("是否立刻填充属性？(y/n) \n");
                if(tolower(getchar()) == 'y')
                {
                    getchar();
                    fillAttr(attrName, stu, config);
                    writeToFile(fileName, stu, config);
                }
                break;
            /*替换属性*/
            case 7:
                printf("请输入要替换的属性名：");
                myfgets(attrName, MAXSTR, stdin);
                printf("请输入新的属性名");
                myfgets(newAttrName, MAXSTR, stdin);
                replaceAttr(attrName, newAttrName,fileName, stu, config);
                printf("是否立刻填充属性？(y/n) \n");
                if(tolower(getchar()) == 'y')
                {
                    getchar();
                    fillAttr(attrName, stu, config);
                    writeToFile(fileName, stu, config);
                }
                break;
            /*填充某一属性*/
            case 8:
                printf("请输入要填充的属性名：");
                myfgets(attrName, MAXSTR, stdin);
                fillAttr(attrName, stu, config);
                writeToFile(fileName, stu, config);
                break;
            /*学生按照序号排序*/
            case 9:
                quickSort(stu, NULL);
                printf("排序成功\n");
                writeToFile(fileName, stu, config);
                break;
            /*删除学生信息*/
            case 10:
                printf("删除学生信息：\n");
                printf("请输入要用于查找的属性名(默认为ID)：");
                myfgetsReturn = myfgets(attrName, MAXSTR-1, stdin);
                if(myfgetsReturn!=NULL && !strcmp(myfgetsReturn, ""))
                    strcpy(attrName,"ID");
                printf("请输入需要查找的关键字(默认为空)：");
                myfgets(info, MAXSTR-1, stdin);
                /*删除*/
                delStu(attrName, info, stu, config);
                break;
            default:
                printf("不明指令\n");
        }
    }while(1);
    return 0;
}
