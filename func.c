#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "head.h"

/*配置文件操作*/

void readConfig(Config *config)
{
    FILE *fp;
    char *filename = config->fileName;
    if((fp = fopen(filename, "r")) == NULL)
    {
        if((fp = fopen(filename, "wt+"))==NULL)
        {
            printf("Cant open config file!\n\a");
            exit(0);
        }
    }
    fread(config, sizeof(Config), 1, fp);
    fclose(fp);
    printf("读取配置成功\n");
}

/*保存配置文件*/
void saveConfig(Config *config)
{
    FILE *fp;
    fp = fopen(config->fileName, "r");
    if(fp == NULL)
    {
        if((fp = fopen(config->fileName, "wt+")) == NULL)
        {
            printf("Cant open config file!\n\a");
            exit(0);
        }
    }

    fwrite(config, sizeof(Config), 1, fp);

    fclose(fp);
    printf("成功保存配置\n");
    return;
}

/*同步配置文件与数据库*/
void syncConfig(char *fileName,Config *config)
{
    FILE *fp;
    int lines=0;
    char buff[1000];

    fp = fopen(fileName, "rt");
    if(fp == NULL)
    {
        printf("Fail to Sync!");
        exit(0);
    }

    /*读取行数*/
    while(fgets(buff, 1000, fp))
    {
        if(isspace(buff[0])) break;
        lines++;
    }
    /*更新保存后返回*/
    config->stuNum=lines-1; /*去除第一行属性列表*/
    saveConfig(config);
    fclose(fp);
}

void setConfig(char *fileName, Config *config)
{
    int i,j;

    strncpy(config->fileName, fileName, MAXSTR);
    fflush(stdin);
    /*设置锁的属性*/
    if(config->attrNum!=0)
    {
        for(i=0;i<config->attrNum;i++)
        {
            printf("%s的Locked, Unset, Show属性:\n", config->attrList[i]);
            for(j=0;j<LOCKNUM;j++)
            {
                if(j==ATTR_LOCKED)
                    printf("Locked:");
                else if(j==ATTR_UNSET)
                    printf("Unset:");
                else if(j==ATTR_SHOW)
                    printf("Show:");
                scanf("%d", &config->lockedAttr[i][j]);
            }
        }
    }
    saveConfig(config);
}

/*-----管理学生数据库-----*/

/*为一个节点分配内存*/
Stu stuDataBaseInit(Stu stuTemp, Config *config)
{
    /*本程序中学生节点都提前分配了下一个节点的内存*/

    int attrNum = config->attrNum;
    int j;
    stuTemp->next = (Stu)calloc(1,sizeof(StuDataBase));
    stuTemp->next->next=NULL;   /*习惯性将指针赋值为空*/
    stuTemp->name = (char *)calloc(MAXSTR,sizeof(char));
    stuTemp->id = (char *)calloc(MAXSTR,sizeof(char));
    stuTemp->attr = (char **)calloc(MAXATTR,sizeof(char *));
    for(j=0;j<attrNum;j++)
    {
        if(config->lockedAttr[ATTR_LOCKED][j]==0)
            stuTemp->attr[j] = (char *)calloc(MAXSTR,sizeof(char));
        else
            stuTemp->attr[j] = NULL;
    }
    stuTemp = stuTemp->next;
    return stuTemp;
}

/*从文件中读入学生数据*/
void readFromFile(char *filename, Stu stu, Config *config)
{
    FILE *fp;
    int i;
    if((fp = fopen(filename, "r")) == NULL)
    {
        if((fp = fopen(filename, "w+")) == NULL)
        {
            printf("Cant open data base!\n\a");
            exit(0);
        }
    }
    if(stu==NULL || feof(fp)) return ;
    while(fgetc(fp)!='\n' && !feof(fp)); /*忽略第一行*/

    for(;stu!=NULL;stu=stu->next)
    {
        fscanf(fp, "%s", stu->id);      /*读id*/
        fscanf(fp, "%s", stu->name);    /*读姓名*/
        /*读取未被锁的并且已设置的属性*/
        for(i=0;i<config->attrNum;i++)
        {
            if(!config->lockedAttr[ATTR_UNSET][i] && !config->lockedAttr[ATTR_LOCKED][i])
            {
                fscanf(fp, "%s", stu->attr[i]);
            }
        }
    }
    fclose(fp);
}

/*将学生数据写入文件*/
void writeToFile(char *filename, Stu stu, Config *config)
{
    FILE *fp;
    int i;
    if((fp = fopen(filename, "w")) == NULL)
    {
        printf("Cant open data base!\n\a");
        exit(0);
    }
    if(feof(fp))
    if(stu==NULL) return ;

    /*写入第一行*/
    fprintf(fp, "StudentID\t");
    fprintf(fp, "Name\t");
    for(i=0;i<config->attrNum;i++)
        fprintf(fp, "%s\t", config->attrList[i]);
    fprintf(fp, "\n");

    /*写入各项信息*/
    for(;stu->next!=NULL;stu=stu->next)
    {
        fprintf(fp, "%s\t", stu->id);
        fprintf(fp, "%s\t", stu->name);
        for(i=0;i<config->attrNum;i++)
        {
            if(!(config->lockedAttr[ATTR_UNSET][i]))
                fprintf(fp, "%s\t", stu->attr[i]);
        }
        fprintf(fp, "\n");
    }
    printf("成功保存数据\n");
    fclose(fp);
}

/*从键盘读入*/
void readFromKey(char *filename, Stu stu, Config *config)
{
    int i;
    int count=0;
    char buff[MAXSTR];
    Stu head = stu;
    while(stu->next != NULL) stu=stu->next;
    while(1)
    {
        /*分配内存*/
        stuDataBaseInit(stu, config);

        /*输入必要属性*/
        printf("ID:");
        myfgets(buff, MAXSTR-1, stdin);
        if(strcmp(buff, "end"))
        {
            strncpy(stu->id,buff, MAXSTR);
        }
        else
        {
            disposeInfo(stu, config);
            break;
        }
        printf("Name:");
        myfgets(stu->name, MAXSTR-1, stdin);

        /*读取自定义属性*/
        for(i=0;i<config->attrNum;i++)
        {
            if(config->lockedAttr[ATTR_LOCKED][i] == 0 &&
                config->lockedAttr[ATTR_UNSET][i] == 0)
            {
                stu->attr[i] = (char *)malloc(sizeof(char)*MAXSTR);
                printf("%s:", config->attrList[i]);
                myfgets(stu->attr[i], MAXSTR-1, stdin);
            }
        }
        count++;
        stu = stu->next;
    }
    config->stuNum+=count;
    /*将更改写入程序*/
    saveConfig(config);
    writeToFile(filename, head, config);
}

/*删除学生*/
void delStu(char *attrName, char *info, Stu stu, Config *config)
{
    Stu p,previous;
    int index;
    int stuFound=0;

    index = indexOfAttr(attrName, config);
    /*判断是否找到属性名*/
    if(index == -1)
        return ;

    for(p=stu;p->next!=NULL;p=p->next)
    {
        if(!strcmp(p->id, info))
        {
            stuFound = 1;
            previous = findPrevious(stu, p);   /*寻找该节点的前一个*/
            previous->next = p->next;   /*将前一个的next指向后一个*/
            p->next = NULL;
            disposeInfo(p, config); /*释放节点*/
        }
    }

    if(!stuFound)
        printf("为找到该学生\n");
}
/*寻找前一节点*/
Stu findPrevious(Stu head, Stu stu)
{
    for(;head->next!=stu && head != NULL;head=head->next);
    return head;
}

/*释放某一节点空间*/
void disposeInfo(Stu stu, Config *config)
{
    int i;
    if(stu->next!=NULL)
    {
        free(stu->next);
        stu->next = NULL;
    }
    free(stu->id);
    free(stu->name);
    for(i=0;i<config->attrNum;i++)
    {
        free(stu->attr[i]);
    }
    free(stu->attr);
}

/*添加一个属性*/
void addAttr(char *attrName, Stu stu, Config *config)
{
    /*最多容纳MAXATTR个属性*/
    if(config->attrNum >= MAXATTR)
    {
        printf("附加属性数量已达到上限%d", MAXATTR);
        return;
    }
    strncpy(config->attrList[config->attrNum], attrName, MAXSTR);

    /*设置锁的属性*/
    config->lockedAttr[ATTR_UNSET][config->attrNum] = 1;
    config->lockedAttr[ATTR_LOCKED][config->attrNum] = 0;
    config->lockedAttr[ATTR_SHOW][config->attrNum] = 1;
    config->attrNum++;
    saveConfig(config);
}

void fillAttr(char *attrName, Stu stu, Config *config)
{
    int i;
    i = indexOfAttr(attrName, config);
    if(i==-1) return;
    while(stu->next != NULL)
    {
        if(stu->attr[i] == NULL)
            stu->attr[i] = (char *)calloc(MAXSTR,sizeof(char));
        printf("%s %s的%s:", stu->id, stu->name, attrName);
        myfgets(stu->attr[i], MAXSTR, stdin);
        stu=stu->next;
    }
    config->lockedAttr[ATTR_UNSET][i] = 0;
    saveConfig(config);
}

/*替换某一属性*/
void replaceAttr(char *attrName,char *newAttrName, char *filename, Stu stu, Config *config)
{
    int index = indexOfAttr(attrName, config);
    /*未找到则返回*/
    if(index==-1)
        return;

    /*替换之*/
    strncpy(config->attrList[index], newAttrName, MAXSTR);
    /*将其标志为未被设置*/
    config->lockedAttr[ATTR_UNSET][index] = 1;
    saveConfig(config);
    writeToFile(filename, stu, config);
}

/*找到属性下标*/
int indexOfAttr(char *attrName, Config *config)
{
    int i;
    for(i=0;i<config->attrNum && config->attrList[i]!=NULL &&
    strcmp(attrName, config->attrList[i]);i++);

    if(i>=config->attrNum || config->attrList[i]==NULL)
    {
        printf("未找到%s\n", attrName);
        return -1;
    }
    return i;
}

/*随机点名函数*/
void rollcall(Stu stu, char *attrName, char *keyword, Config *config)
{
    int i,j;
    int attrFound = 0;
    int stuFound = 0;
    int stuNum = config->stuNum;
    Stu *matchList = (Stu *)calloc(stuNum,sizeof(Stu));
    srand(time(0));
    /*按学号查找*/
    if(!strcmp("ID", attrName))
    {
        attrFound = 1;
        /*找到随机的学生*/
        for(i=0;stu->next!=NULL;stu=stu->next)
        {
            if(strstr(stu->id, keyword))
            {
                matchList[i] = stu;
                i++;
            }
        }
        if(i)
        {
            stuFound = 1;
            stu = matchList[rand()%(i-1)];
            printf("%s含%s的随机结果：%s\t%s同学\n", attrName, keyword, stu->id, stu->name);
        }
    }

    /*按姓名查找*/
    else if(!strcmp("Name", attrName))
    {
        attrFound = 1;
        /*找到随机的学生*/
        for(i=0;stu->next!=NULL;stu=stu->next)
        {
            if(strstr(stu->name, keyword) != NULL)
            {
                matchList[i] = stu;
                i++;
            }
        }
        if(i)
        {
            stuFound = 1;
            if(i!=1)
                stu = matchList[rand() % (i-1)];
            else
                stu = matchList[0];
            printf("%s含%s的随机结果：%s\t%s同学\n", attrName, keyword, stu->id, stu->name);
        }
    }

    /*查找自定义属性*/
    else
    {
        /*寻找属性名对应序号*/
        i = indexOfAttr(attrName, config);
        /*找到随机的学生*/
        if(i!=-1)
        {
            attrFound = 1;
            for(j=0;stu->next!=NULL;stu=stu->next)
            {
                if(strstr(stu->attr[i], keyword))
                {
                    matchList[j] = stu;
                    j++;
                }
            }
            if(j)
            {
                stuFound = 1;
                stu = matchList[rand()%(j-1)];
                printf("%s含%s的随机结果：%s\t%s同学\n", attrName, keyword, stu->id, stu->name);
            }
        }
    }

    /*判断是否找到属性*/
    if(!attrFound)
    {
        printf("The Attribution Name %s not found\n", attrName);
    }
    else if(!stuFound)
    {
        printf("The keyword %s not found\n", keyword);
    }
}

/*打印菜单信息*/
void showMenu()
{
    printf("******欢迎使用点名系统******\n");
    printf("0.输入0结束程序\n");
    printf("1.按条件点名\n");
    printf("2.从键盘读入学生信息\n");
    printf("3.显示学生信息\n");
    printf("4.进行设置\n");
    printf("5.查看所有属性\n");
    printf("6.添加属性\n");
    printf("7.替换属性\n");
    printf("8.填充属性\n");
    printf("9.按学生学号降次排序\n");
    printf("请输入需要执行的功能:\n");
}

/*方便用户查看已有属性*/
void showAttr(Config *config)
{
    int attrNum = config->attrNum;
    int i;
    char *lockedStatus[]={"Unlocked", "Locked"};
    char *unsetStatus[]={"Setted", "Unset"};
    char *showStatus[]={"Hide", "Show"};
    printf("1.ID\t2.Name\n");
    for(i=0;i<attrNum;i++)
    {
        printf("%d.%s", i+3, config->attrList[i]);
        printf("(%s %s %s)\n", lockedStatus[config->lockedAttr[ATTR_LOCKED][i]],
               unsetStatus[config->lockedAttr[ATTR_UNSET][i]],
               showStatus[config->lockedAttr[ATTR_SHOW][i]]);
    }
    printf("\n");
}

/*显示学生数据*/
void showData(Stu stu, Config *config)
{
    int j;
    int attrNum = config->attrNum;

    printf("ID\t\t\tName\t\t");
    for(j=0;j<attrNum;j++)
    {
        /*若属性被锁上则不显示*/
        if(config->lockedAttr[ATTR_UNSET][j] == 1 || config->lockedAttr[ATTR_SHOW][j] == 0)
            continue;
        else
            printf("%s\t\t", config->attrList[j]);
    }
    printf("\n");

    for(;stu->next!=NULL;stu=stu->next)
    {
        printf("%s\t\t%s\t\t", stu->id, stu->name);
        for(j=0;j<attrNum;j++)
        {
            /*若属性被锁上则不显示*/
            if(config->lockedAttr[ATTR_LOCKED][j] ||
               config->lockedAttr[ATTR_UNSET][j] || !config->lockedAttr[ATTR_SHOW][j])
                continue;
            else
                printf("%s\t\t", stu->attr[j]);
        }
        printf("\n");
    }
}

/*给学生序号排序*/
/*主要排序*/
void quickSort(Stu pBegin, Stu pEnd)
{
    if(pBegin->next!=pEnd && pBegin!=pEnd)
    {
        Stu partion = getPartion(pBegin, pEnd);
        /*分两边*/
        quickSort(pBegin, partion);
        /*partion->next可能为空，先判断*/
        if(partion->next->next)
            quickSort(partion->next, pEnd);
    }
}
/*获得分割点*/
Stu getPartion(Stu pBegin, Stu pEnd)
{
    char buff[MAXSTR];
    strncpy(buff, pBegin->id, MAXSTR);
    Stu p = pBegin;
    Stu q = pBegin->next;
    while(q->next!=pEnd)
    {
        /*pBegin前的id都要小于p的id,pBegin后的自然大于等于*/
        if(strcmp(q->id, buff) < 0)
        {
            p = p->next;
            swapStu(p, q);
        }
        q = q->next;
    }
    /*最后把pBegin换到中间值点处*/
    swapStu(p, pBegin);
    return p;
}
/*交换学生数据*/
void swapStu(Stu stu1, Stu stu2)
{
    StuDataBase temp;
    /*先整块交换*/
    temp = *stu1;
    *stu1 = *stu2;
    *stu2 = temp;
    /*再把指针换回来*/
    temp.next = stu1->next;
    stu1->next = stu2->next;
    stu2->next = temp.next;
    /*这样做整块交换减少读写次数，并且以后更改结构体时无需修改此处*/
}

/*定义一个可去空格的fgets*/
char *myfgets(char *buff, int n, FILE *fp)
{
    char *returnPoint = fgets(buff, n, fp);
    /*去空格*/
    if(buff[strlen(buff)-1]=='\n')
        buff[strlen(buff)-1]='\0';
    else
        while(getchar() != '\n');
    return returnPoint;
}
