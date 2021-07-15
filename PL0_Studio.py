import tkinter as tk  # 使用Tkinter前需要先导入
import os
import pathlib
# 第1步，实例化object，建立窗口window
window = tk.Tk()
 
# 第2步，给窗口的可视化起名字
window.title('extend-pl0')
 
# 第3步，设定窗口的大小(长 * 宽)
window.geometry('800x800')  # 这里的乘是小x

def importfile():
    filepath=getFileName()
    if os.path.exists(filepath):
        a = open(filepath, 'r', encoding='utf-8')
        t1.delete("0.0","end")
        for id_names in a:
            t1.insert('insert', id_names)
        a.close()

def getFileName():
    filename=filenameText.get()    
    filepath=pathlib.Path(filename)
    return filepath

def save():
    filepath=getFileName()
    a=open(filepath,'w',encoding='gbk')
    codetext=t1.get("0.0","end")
    a.write(codetext)
    a.close()

def compile():
    filename=filenameText.get()
    #executeList=filename.split(".",1)
    #programName=executeList[0]
    os.system("extend-pl0 "+filename)
    a=open('pcode.txt','r',encoding='utf-8')
    t2.delete("0.0","end")
    for setences in a:
        t2.insert('insert',setences)
    a.close()

def interpret():
    os.system("interpret")   

filenameText=tk.Entry(window,bd=5)
filenameText.place(x=30,y=30)
b2 = tk.Button(window, text='打开', width=8,
               height=1,command=importfile)
b2.place(x=200,y=30)
b3 = tk.Button(window, text='保存', width=8,
               height=1,command=save)
b3.place(x=300,y=30)
b4 = tk.Button(window, text='编译', width=8,
               height=1,command=compile)
b4.place(x=400,y=30)
b5 = tk.Button(window, text='运行', width=8,
               height=1,command=interpret)
b5.place(x=500,y=30)

t1 = tk.Text(window, height=33,width=60)
t1.place(x=30,y=100)

t2 = tk.Text(window, height=33,width=30)
t2.place(x=500,y=100)
# 第8步，主窗口循环显示
window.mainloop()