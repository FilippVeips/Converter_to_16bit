#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //FileInput();
    //TextOutput();
}

//#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <libxl.h>
#include <Windows.h>
#include <vector>

//using namespace System;
using namespace std;
using namespace libxl;

std::vector<string> globvec;

struct Date
{
public:

    int hours;
    int minutes;
    int day;
    int month;
    int year;
};

bool checker(Date& Current) {
    if (Current.hours > 23 || Current.hours < 0 || Current.minutes > 59 || Current.minutes < 0 || Current.month > 12 || Current.month < 0 || Current.day < 0 || Current.year < 0 || Current.day >31)
        return 0;
    else {
        if ((Current.month == 4 || Current.month == 6 || Current.month == 9 || Current.month == 11) & Current.day > 30)
            return 0;
        else {
            if (((Current.year % 4 != 0 || (Current.year % 100 == 0 & Current.year % 400 != 0)) & Current.month == 2 & Current.day > 28) || ((Current.year % 4 == 0 || (Current.year % 100 != 0 & Current.year % 400 == 0)) & Current.month == 2 & Current.day > 29))
                return 0;
            else
                return 1;
        }
    }

}

string transformator(string str) {
    string answer;
    string subanswer;
    for (unsigned int i = 0; i != str.size(); i++) {
        stringstream hexstream;
        string hexstring;
        int num;
        num = (int)str[i];
        num = num-48 + 25;
        hexstream << hex << num;
        getline(hexstream, hexstring);
        subanswer = subanswer + hexstring;
    }
    for (int i = 0; i != subanswer.size(); i++) {
        if (isalpha(subanswer[i])) subanswer[i] = toupper(subanswer[i]);
        answer = answer + subanswer[i];
    }
    //std::cout << answer<<"\n";

    return answer;
}

void Output(string answer, Date Current, string strcounter,Sheet* sheet) {
    string hstring= to_string(Current.hours);
    string mstring= to_string(Current.minutes);
    string strdate = to_string(Current.day) +"."+ to_string(Current.month) + "." + to_string(Current.year);
    if (hstring.size() == 1) hstring = "0" + hstring;
    if (mstring.size() == 1) mstring = "0" + mstring;
    string strtime = hstring + ":" + mstring;
    wstring widedate = wstring(strdate.begin(), strdate.end());
    wstring widetime = wstring(strtime.begin(), strtime.end());
    wstring widecounter = wstring(strcounter.begin(), strcounter.end());
    wstring wideanswer = wstring(answer.begin(), answer.end());
    const wchar_t* ansdate = widedate.c_str();
    const wchar_t* anstime = widetime.c_str();
    const wchar_t* anscounter = widecounter.c_str();
    const wchar_t* lastanswer = wideanswer.c_str();
    int counter = stoi(strcounter);
    sheet->writeStr(counter, 0, ansdate);
    sheet->writeStr(counter, 1, anstime);
    sheet->writeStr(counter, 2, anscounter);
    sheet->writeStr(counter, 3, lastanswer);
}

void fileinput(istream& input,Sheet* sheet, int counter) {
    while (!input.eof()) {
        Date Current;
        string datestring;
        string stringline;
        getline(input, stringline);
        stringstream linestream(stringline);
        string word;
        getline(linestream, word, ':');
        Current.hours = stoi(word);
        datestring = datestring + word;
        getline(linestream, word, ',');
        Current.minutes = stoi(word);
        datestring = datestring + word;
        getline(linestream, word, '.');
        Current.day = stoi(word);
        word = to_string(Current.day);
        datestring = datestring + word;
        getline(linestream, word, '.');
        Current.month = stoi(word);
        datestring = datestring + word;
        getline(linestream, word);
        Current.year = stoi(word);
        datestring = datestring + word;
        counter++;
        string strcounter=to_string(counter);
        int strsize = strcounter.size();
        if (strsize > 10){
            globvec.push_back("too much input");
            return;
        }
        else {
            if (strsize % 2 != 0) strcounter = "0" + strcounter;
            datestring = datestring + strcounter;
        }
        //std::cout << Current.hours << " " << Current.minutes << " " << Current.day << " " << Current.month << " " << Current.year << " ";
        //std::cout << datestring<<"\t";
        if (checker(Current) == 0) {
            globvec.push_back("Wrong date");
            counter--;
            continue;
        }
        else
        {
            string answer=transformator(datestring);
            globvec.push_back(answer);
            Output(answer,Current,strcounter,sheet);
        }
    }
}

std::string handinput(Sheet* sheet, string h, string mi,string d,string mo, string y, int counter) {
    string datestring;
    Date Current;
    Current.hours = stoi(h);
    datestring = datestring + h;
    Current.minutes = stoi(mi);
    datestring = datestring + mi;
    Current.day = stoi(d);
    datestring = datestring + d;
    Current.month = stoi(mo);
    datestring = datestring + mo;
    Current.year = stoi(y);
    datestring = datestring + y;
    counter++;
    string strcounter = to_string(counter);
    int strsize = strcounter.size();
    if (strsize > 10) {
        return "too much input";
    }
    else {
      if (strsize % 2 != 0) strcounter = "0" + strcounter;
         datestring = datestring + strcounter;
      }
      //std::cout << Current.hours << " " << Current.minutes << " " << Current.day << " " << Current.month << " " << Current.year << " ";
      //std::cout << datestring << "\t";
      if (checker(Current) == 0) {
         counter--;
         return "Wrong date";
         }
      else
         {
           string answer = transformator(datestring);
           Output(answer, Current,strcounter,sheet);
           return answer;
         }
}

int outfileread(Sheet* sheet) {
    if (sheet) {
    int counter = 0;
    for (int row = sheet->firstRow(); row < sheet->lastRow(); ++row)
       {
            counter++;
       }
       return counter;
    }
}

void filemain(QString filename,QString Tableoutput)
{
    std::string tableout = Tableoutput.toLocal8Bit().constData();
    Book* book = xlCreateXMLBook();
    wstring widename = wstring(tableout .begin(), tableout .end());
    const wchar_t* wname = widename.c_str();
    if (book->load(wname)) {
        Sheet* sheet = book->getSheet(0);
        int outcounter = outfileread(sheet);
        if (outcounter >= 1) outcounter--;

        std::string name = filename.toLocal8Bit().constData();
        ifstream inpFile;
        inpFile.open(name, ios_base::in);
        fileinput(inpFile, sheet, outcounter);
        //book->save(L"Output.xls");
        book->save(wname);
        book->release();
    }
}

std::string handmain(QString h,QString mi,QString d,QString mo,QString y,QString Outputline)
{
    std::string outline = Outputline.toLocal8Bit().constData();
    Book* book = xlCreateXMLBook();
    wstring widename = wstring(outline.begin(), outline.end());
    const wchar_t* wname = widename.c_str();
    if (book->load(wname)) {
        Sheet* sheet = book->getSheet(0);
        int outcounter = outfileread(sheet);
        if (outcounter >= 1) outcounter--;

        std::string sh = h.toLocal8Bit().constData();
        std::string smi = mi.toLocal8Bit().constData();
        std::string sd = d.toLocal8Bit().constData();
        std::string smo = mo.toLocal8Bit().constData();
        std::string sy = y.toLocal8Bit().constData();
        std::string ansstr=handinput(sheet,sh,smi,sd,smo,sy,outcounter);
        book->save(wname);
        book->release();
        return ansstr;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::FileTextOutput(string answer)
 {
    QString qans = QString::fromStdString(answer);
    ui->fileoutput->append(qans);
    QTextCursor cursor = ui->fileoutput->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
 }

void MainWindow::HandTextOutput(string answer)
 {
    QString qans = QString::fromStdString(answer);
    ui->handoutput->append(qans);
    QTextCursor cursor = ui->handoutput->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
 }

void MainWindow::on_findfileButton_clicked()
{
    QString filename = ui->fileline->text();
    QString Tableout = ui->Tableoutput->text();
    filemain(filename,Tableout);
    for (int i=0;i!=globvec.size();i++){
        FileTextOutput(globvec[i]);
    }
    globvec.clear();
    ui->fileline->clear();
}

void MainWindow::on_inputButton_clicked()
{
   QString hours = ui->hourline->text();
   QString minutes = ui->minuteline->text();
   QString day = ui->dayline->text();
   QString month = ui->monthline->text();
   QString year = ui->yearline->text();
   QString Outputl = ui->Outputline->text();
   std::string ans=handmain(hours,minutes,day,month,year,Outputl);
   HandTextOutput(ans);
   ui->hourline->clear();
   ui->minuteline->clear();
   ui->dayline->clear();
   ui->monthline->clear();
   ui->yearline->clear();
}
