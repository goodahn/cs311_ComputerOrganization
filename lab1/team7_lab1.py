import sys
import struct
data_size=0
text_size=0
data_index=0
text_index=0
data_list=[]
text_list=[]
all_list=[]
label_list={}
def _bin(n,l):
    return ''.join(str(1&n>>i) for i in range(l)[::-1])
def get_size():
    global data_index
    global text_index
    global data_size
    global text_size
    global all_list
    for i in range(len(all_list)):
        if all_list[i]==['.data']:
            data_index=i
            print 'data seg'
        elif all_list[i]==['.text']:
            text_index=i
            print 'text seg'
    data_size = text_index - data_index -1
    data_size *= 4
    for i in xrange( text_index, len(all_list)):
        if len(all_list[i])>1:
            text_size+=1
    text_size *= 4
def bin_five(a):
    return _bin(a, 5) #'0'*( 5 - len( bin(a)[2:] ))+bin(a)[2:]
def bin_sixteen(a):
    '''if a<0:
        return bin(0x10000+a)[2:]
    return '0'*( 16 - len( bin(a)[2:]))+bin(a)[2:]'''
    return _bin(a, 16)
def bin_26(a):
    return _bin(a, 26) #'0'*( 26 - len( bin(a)[2:]))+bin(a)[2:]
def asm_to_bin( a ):
    if a[0][0] == 'addiu':
        b = '001001'+bin_five(int(a[0][2][1:]))+bin_five(int(a[0][1][1:]))+bin_sixteen(int(a[0][3]))
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'addu':
        b = '000000'+bin_five(int(a[0][2][1:]))+bin_five(int(a[0][3][1:]))+bin_five(int(a[0][1][1:]))+'00000100001'
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'and':
        b = '000000'+bin_five(int(a[0][2][1:]))+bin_five(int(a[0][3][1:]))+bin_five(int(a[0][1][1:]))+'00000100100'
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'andi':
        b = '001100'+bin_five(int(a[0][2][1:]))+bin_five(int(a[0][1][1:]))+bin_sixteen(int(a[0][3]))
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'beq':#important
        try:
            b = '000100'+bin_five(int(a[0][1][1:]))+bin_five(int(a[0][2][1:]))+bin_sixteen(int(a[0][3])-int(a[1]))
            #b = int(b,2)
            #b = struct.pack(">I",b)
            return b
        except:
            b = '000100'+bin_five(int(a[0][1][1:]))+bin_five(int(a[0][2][1:]))+bin_sixteen(int(label_list[a[0][3]])-int(a[1]))
            #b = int(b,2)
            #b = struct.pack(">I",b)
            return b
    elif a[0][0] == 'bne':#important
        try:
            b = '000101'+bin_five(int(a[0][1][1:]))+bin_five(int(a[0][2][1:]))+bin_sixteen(int(a[0][3])-int(a[1]))
            #b = int(b,2)
            #b = struct.pack(">I",b)
            return b
        except:
            b = '000101'+bin_five(int(a[0][1][1:]))+bin_five(int(a[0][2][1:]))+bin_sixteen(int(label_list[a[0][3]])-int(a[1]))
            #b = int(b,2)
            #b = struct.pack(">I",b)
            return b
    elif a[0][0] == 'j':#important
        try:
            b = '000010'+bin_26(int(a[0][1]))
            #b = int(b,2)
            #b = struct.pack(">I",b)
            return b
        except:
            b = '000010'+bin_26(label_list[a[0][1]])
            #b = int(b,2)
            #b = struct.pack(">I",b)
            return b
    elif a[0][0] == 'jal':#important
        try:
            b = '000011'+bin_26(int(a[0][1]))
            #b = int(b,2)
            #b = struct.pack(">I",b)
            return b
        except:
            b = '000011'+bin_26(label_list[a[0][1]])
            #b = int(b,2)
            #b = struct.pack(">I",b)
            return b
    elif a[0][0] == 'jr':#important
        b = '000000'+bin_five(int(a[0][1][1:]))+'000000000000000001000'
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'lui':
        try:
            b = '00111100000'+bin_five(int(a[0][1][1:]))+bin_sixteen(int(a[0][2]))
            #b = int(b,2)
            #b = struct.pack(">I",b)
            return b
        except:
            b = '00111100000'+bin_five(int(a[0][1][1:]))+bin_sixteen(int(label_list[a[0][2]])-int(a[1]))
            #b = int(b,2)
            #b = struct.pack(">I",b)
            return b
    elif a[0][0] == 'lw':
        b = '100011'+bin_five(int(a[0][3][1:]))+bin_five(int(a[0][1][1:]))+bin_sixteen(int(a[0][2]))
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'nor':
        b = '000000'+bin_five(int(a[0][2][1:]))+bin_five(int(a[0][3][1:]))+bin_five(int(a[0][1][1:]))+'00000100111'
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'or':
        b = '000000'+bin_five(int(a[0][2][1:]))+bin_five(int(a[0][3][1:]))+bin_five(int(a[0][1][1:]))+'00000100101'
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'ori':
        b = '001101'+bin_five(int(a[0][2][1:]))+bin_five(int(a[0][1][1:]))+bin_sixteen(int(a[0][3]))
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'sltiu':
        b = '001011'+bin_five(int(a[0][2][1:]))+bin_five(int(a[0][1][1:]))+bin_sixteen(int(a[0][3]))
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'sltu':
        b = '000000'+bin_five(int(a[0][2][1:]))+bin_five(int(a[0][3][1:]))+bin_five(int(a[0][1][1:]))+'00000101011'
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'sll':
        b = '00000000000'+bin_five(int(a[0][2][1:]))+bin_five(int(a[0][1][1:]))+bin_five(int(a[0][3]))+'000000'
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'srl':
        b = '00000000000'+bin_five(int(a[0][2][1:]))+bin_five(int(a[0][1][1:]))+bin_five(int(a[0][3]))+'000010'
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'sw':
        b = '101011'+bin_five(int(a[0][3][1:]))+bin_five(int(a[0][1][1:]))+bin_sixteen(int(a[0][2]))
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
    elif a[0][0] == 'subu':
        b = '000000'+bin_five(int(a[0][2][1:]))+bin_five(int(a[0][3][1:]))+bin_five(int(a[0][1][1:]))+'00000100011'
        #b = int(b,2)
        #b = struct.pack(">I",b)
        return b
if __name__=='__main__':
    input_file = open( sys.argv[1] ,'r' )
    b = input_file.readline()
    if b!='\n':
        all_list.append(b[:-1].split())
    while b!='':
        b = input_file.readline()
        if b[:-1].split()!=[]:
            all_list.append(b[:-1].split())
    input_file.close()
    get_size()
    data_list = all_list[data_index+1:text_index]
    for i in range(len(data_list)):
        if data_list[i][-1][:2]=='0x':
            data_list[i][-1]=str(int(data_list[i][-1][2:],16))
        elif data_list[i][-1][1:3] == '0x':
            data_list[i][-1]='-'+str(int(data_list[i][-1][3:],16))
        if len(data_list[i])>2:
            label_list[data_list[i][0][:-1]] = (0x10000000+i*4)
        data_list[i] = (data_list[i],0x10000000+i*4)
    text_list = all_list[text_index+1:]
    text_addr = 0x400000
    num_la=0
    print text_list
    print data_list
    len_text_list = len(text_list)
    for i in range(len_text_list):
        if len(text_list[i+num_la])<2:
            label_list[text_list[i+num_la][0][:-1]]=text_addr/4
            text_list[i+num_la] = ( text_list[i+num_la], text_addr/4 )
        else:
            for j in range(len(text_list[i+num_la])):
                text_list[i+num_la][j]=text_list[i+num_la][j].replace(',','')
            if text_list[i+num_la][-1][:2] == '0x':
                text_list[i+num_la][-1]=str(int(text_list[i+num_la][-1][2:],16))
            elif text_list[i+num_la][-1][1:3] == '0x':
                text_list[i+num_la][-1]='-'+str(int(text_list[i+num_la][-1][3:],16))
            if text_list[i+num_la][0] == 'la':
                if int(label_list[text_list[i+num_la][2]]) & 0xffff ==0:
                    text_list[i+num_la][0]='lui'
                    text_list[i+num_la][2]=(int(label_list[text_list[i+num_la][2]])&0xffff0000 )>>16
                else:
                    text_list[i+num_la][0]='lui'
                    low_address=0
                    text_size+=4
                    register = text_list[i+num_la][1]
                    try:
                        low_address = str(int(text_list[i+num_la][2])&0xffff)
                        text_list[i+num_la][2] = str((int(text_list[i+num_la][2])&0xffff0000>>16))
                    except:
                        low_address = str(int(label_list[text_list[i+num_la][2]])&0xffff)
                        text_list[i+num_la][2] = str((int(label_list[text_list[i+num_la][2]])&0xffff0000)>>16)
                    text_list[i+num_la] = ( text_list[i+num_la], text_addr/4+1 )
                    text_addr+=4
                    text_list = text_list[:i+num_la+1]+[(['ori',register,register, low_address] ,text_addr/4+1 )] +text_list[i+num_la+1:]
                    text_addr+=4
                    num_la+=1
                    continue
            if text_list[i+num_la][0] == 'lw' or text_list[i+num_la][0] == 'sw':
                temp = text_list[i+num_la][2].split('(')
                text_list[i+num_la][2] = temp[0]
                text_list[i+num_la].append(temp[1][:-1])
            text_list[i+num_la] = ( text_list[i+num_la], text_addr/4+1 )
            text_addr+=4
#data_list
    print data_size, text_size
    print data_list
    print text_list
    print label_list
    f = open( sys.argv[1][:-1]+'o','w')
    f.write(_bin(text_size, 32))
    f.write(_bin(data_size, 32))
    for i in text_list:
        binary = asm_to_bin(i)
        if binary !=None:
            f.write(binary)
    for i in data_list:
        f.write(_bin(int(i[0][-1]),32))
    f.close()

