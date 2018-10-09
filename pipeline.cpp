#include<iostream>
#include<fstream>
#include<cstdlib>
#include<math.h>
#include<stdlib.h>
using namespace std;
string instructions[100]; //save the instructios
int mem[5]; // simulated the memory
int reg[10]; //simulated the register
int clockcycle=0; // simulated the clock cycle
ofstream fout("Result.txt");

///deal with hazard
bool lwhz=false;
bool bneq=false;
bool flu=false;

///for instruction fetch variable
int pc = 0;
bool ifid_input = false;
string finstruction="00000000000000000000000000000000";

///for insruction decode and register fectch variable
int idpc=0;
string op="000000";
string fun="000000";
string idexsignal="000000000";
bool idex_input=0;
int readdata1=0;
int readdata2=0;
int signextend=0;
long int rs=0;
int rt=0;
int rd=0;

///for execution variable
bool exe_input=false;
int aluout=0;
int writedata=0;
int exert=0;
int exepc=0;
int temp=0;
int temp2=0;
string exesignal="000000000";
string aluop="00";

///for read or write memory variable
bool meminput=false;
bool membranch=false;
int memreaddata=0;
int memalu=0;
int memrt=0;
string memsignal="00";
/// for wb
int wbrd=0;

int b_d(string str, bool sign)//binary to decimal
{
    bool f = false;
    int ans = 0;
    for (int i = 0; i < str.length(); i++)
    {
        ans = ans*2 + str[i] - '0';
        if (i == 0 && str[i] == '1')
            f = true;
    }
    if (f&&sign)//if it is a signinteger
    {
        ans = -(ans);
    }
    return ans;
}
void print();

void flush()
{
    //run the bne hazard happen clock cycle with this function
    ifid_input = true;
    pc = exepc+4;
    if(instructions[(pc/4)]=="")// pc/4= the number of instruction
    {
        finstruction="00000000000000000000000000000000";//no instruction
        ifid_input=false;
    }
    else
    {
        finstruction=instructions[(pc/4)];//fetch the instruction
    }

    //id_exe
    idex_input = true;
    readdata1 =0 ;
    readdata2 = 0;
    signextend = 0;
    rs = 0;
    rt = 0;
    rd = 0;
    idpc = 0;
    idexsignal = "000000000";
    print();
}
void detect()
{
    if(exert==rs && exert != 0 && exesignal[3] == '1')                   //forwarding for data hazard A 10
        readdata1 = aluout;
    if(exert == rt && exert != 0 && exesignal[3] == '1')                    //forward B 10
        readdata2 = aluout;

    if(memrt == rs && memrt != 0 && memsignal[0] == '1')
        readdata1 = (memsignal[1] == '1') ? memreaddata : memalu;          //ForwardA01
    if(memrt== rt && memrt != 0 && memsignal[0] == '1')
        readdata2 = (memsignal[1] == '1') ? memreaddata : memalu;    //ForwardB01

    if (idexsignal[5] == '1'&& (rt == b_d(finstruction.substr(6, 5),false)||rt==b_d(finstruction.substr(11, 5),false)))//lw hazard
    {
        // set NOP
        lwhz=true;
        pc -= 4;
        ifid_input = true;
    }

}
void writeback(string wbsignal, int rt , int alu, int readata)
{
    if (rt ==0)//$0 cant overwrite
        return;
    if(wbsignal[0]=='1')
    {
        if(wbsignal[1]=='0')//R-type
        {
            reg[rt]=alu;
        }
        else if(wbsignal[1]=='1')//lw
        {
            reg[rt]=readata;
        }
    }
    else if(wbsignal[0]=='0')//sw and beq
    {
        //NOP
        return ;
    }
}
void instructiondecode()
{
    idex_input=ifid_input;
    ifid_input=false;
    idpc=pc;
    op=finstruction.substr(0,6);//first 6 bits
    if(op=="000000")//r type
    {
        rs=b_d(finstruction.substr(6,5),false);
        rt=b_d(finstruction.substr(11,5),false);
        rd=b_d(finstruction.substr(16,5),false);
        fun=finstruction.substr(26);
        readdata1=reg[rs];
        readdata2=reg[rt];
        //r type no singextend
        signextend=b_d(finstruction.substr(16,16),true);
        if(rs==0&&rt==0&&rd==0&&fun=="000000"||lwhz==true)//no instruction
        {
            idexsignal="000000000";
            lwhz=false;

        }
        else
        {
            idexsignal="110000010";
        }

    }
    else//i type
    {
        rs=b_d(finstruction.substr(6,5),false);
        rt=b_d(finstruction.substr(11,5),false);
        readdata1=reg[rs];
        readdata2=reg[rt];
        signextend=b_d(finstruction.substr(16,16),true);
        //i type no
        rd=0;
        if(op=="100011")//lw
        {
            idexsignal="000101011";
        }
        else if(op=="101011")//sw
        {
            idexsignal="100100101";
        }
        else if(op=="001000")//addi
        {
            idexsignal="000100010";
        }
        else if(op=="001100")//andi
        {
            idexsignal="011100010";
        }
        else if(op=="000101")//bne
        {
            idexsignal="101010001";
        }
    }
}
void execution()
{
    exe_input=idex_input;
    idex_input=false;

    aluop=idexsignal.substr(1,2);
    exesignal=idexsignal.substr(4,5);
    writedata=readdata2;//for sw, not always use
    if(idexsignal[0]=='1'&&idexsignal[1]=='1')//r type
    {
        exert=rd;
    }
    else
    {
        exert=rt;
    }
    temp=readdata1;
    if(idexsignal[3]=='0')//alusrc== 0
        temp2=readdata2;
    else //alusrc==0
        temp2=signextend;
    if(aluop=="10")//r type
    {

        if(fun=="100000" )//add
            aluout=temp+temp2;
        else if(fun== "100100" )//and
            aluout=temp&temp2;
        else if(fun== "100101" )//or
            aluout=temp|temp2;
        else if(fun== "100010" )//sub
            aluout=temp-temp2;
        else if(fun== "101010" )//slt
        {
            if (temp<temp2)
                aluout=1;
            else
                aluout=0;
        }
        else if(fun== "000000" )
            aluout=0;
    }
    else if(aluop=="00")//lw&sw
        aluout=temp+temp2;
    else if(aluop=="01")//bNeq
    {
        aluout=temp-temp2;
        bneq=true;
        if(aluout!=0)
            flu=true;
    }

    else if(idexsignal=="000100010")//addi
    {
        aluout=temp+temp2;

    }
    else if(idexsignal=="011100010")//andi
    {
        aluout=temp&temp2;
    }
    exepc=idpc+(signextend*4);

}
void memoryaccess()
{
    meminput=exe_input;
    exe_input=false;
    membranch=false;
    memrt=exert;
    memalu=aluout;
    memsignal=exesignal.substr(3,2);
    if (memsignal == "01" && memalu != 0&&bneq==true) //bneq
    {
        bneq=false;
        membranch = true;
    }
    if(memsignal == "11")    //load
    {
        memreaddata=mem[memalu/4];
    }
    else
    {
        memreaddata=0;
    }
    if(memsignal == "01"&&exesignal[2]=='1')//sw
    {
        mem[memalu/4]=writedata;
    }

}
void instructionfetch()
{
    pc=pc+4;
    ifid_input=true;
    if(instructions[(pc/4)]=="")// pc/4= the number of instruction
    {
        finstruction="00000000000000000000000000000000";//no instruction
        ifid_input=false;
    }
    else
    {
        finstruction=instructions[(pc/4)];//fetch the instruction
    }
}
bool gocycling()
{
    wbrd=memrt;
    writeback(memsignal,memrt,memalu,memreaddata);
    memoryaccess();
    execution();
    instructiondecode();
    instructionfetch();
    clockcycle++;
    wbrd=memrt;
    //beq hazard
    if (flu==true)
    {
        flu=false;
        flush();
    }
    else
        {print();}
    detect();

    if((memsignal=="00"&&memalu == 0 && memreaddata==0)&&exesignal=="00000"&&idexsignal=="000000000"&&clockcycle!=1)
    {
        return false;
    }
    else//continue
        return true;
}
int main()
{
    //initialize the memory and registers
    mem[0]=5;
    mem[1]=9;
    mem[2]=4;
    mem[3]=8;
    mem[4]=7;

    reg[0]=0;
    reg[1]=9;
    reg[2]=8;
    reg[3]=7;
    reg[4]=1;
    reg[5]=2;
    reg[6]=3;
    reg[7]=4;
    reg[8]=5;
    reg[9]=6;

    //file open
    ifstream fin("Input.txt");
    int i=1;// start with 1 for convient
    //read the input instructions
    while (fin >> instructions[i])
        i++;

    //run by function gocycking
    while(gocycling()==true);

    fin.close();
    fout.close();
    return 0;
}
void print()
{
    fout<<"CC "<<clockcycle<<":"<<endl;
    fout<<endl;
    fout<<"Registers:"<<endl;
    for(int k=0; k<10; k++)
        fout<<"$"<<k<<": "<<    reg[k]<<endl;
    fout<<endl;
    fout<<"Data memory:"<<endl;
    fout<<"0x00: "<<mem[0]<<endl;
    fout<<"0x04: "<<mem[1]<<endl;
    fout<<"0x08: "<<mem[2]<<endl;
    fout<<"0x0C: "<<mem[3]<<endl;
    fout<<"0x10: "<<mem[4]<<endl;
    fout<<endl;
    fout<<"IF/ID :"<<endl;
    fout<<"PC              "<<pc<<endl;
    fout<<"Instruction     "<<finstruction<<endl;
    fout<<endl;
    fout<<"ID/EX :"<<endl;
    fout<<"ReadData1       "<<readdata1<<endl;
    fout<<"ReadData2       "<<readdata2<<endl;
    fout<<"sign_ext        "<<signextend<<endl;
    fout<<"Rs              "<<rs<<endl;
    fout<<"Rt              "<<rt<<endl;
    fout<<"Rd              "<<rd<<endl;
    fout<<"Control signals "<<idexsignal<<endl;
    fout<<endl;
    fout<<"EX/MEM :"<<endl;
    fout<<"ALUout          "<<aluout<<endl;
    fout<<"WriteData       "<<writedata<<endl;
    fout<<"Rt/Rd           ";
    fout<<exert<<endl;
    fout<<"Control signals "<<exesignal<<endl;
    fout<<endl;
    fout<<"MEM/WB :"<<endl;
    fout<<"ReadData        "<<memreaddata<<endl;
    fout<<"ALUout          "<<memalu<<endl;
    fout<<"Rt/Rd           "<<wbrd<<endl;
    fout<<"Control signals "<<memsignal<<endl;
    fout<<"================================================================="<<endl;
}
