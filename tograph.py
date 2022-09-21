
from decimal import ROUND_FLOOR
import sys
import math


def convertToCudaCutsFormat(C, N, H, W, dstfile):
    with open(dstfile, "w") as wfile:
        wfile.write(H + " " + W + " " + "2\n")

        #dummy output
        for i in range(H):
            for j in range(W):
                wfile.write(j+ " ")
            wfile.write("\n")

        #sink and target weights
        for i in range(H):
            for j in range(W):
                wfile.write(C[0][1 + j + i * W]+ " ")
            wfile.write("\n")

        for i in range(H):
            for j in range(W):
                wfile.write(C[1 + j + i * W][N-1]+ " ")
            wfile.write("\n")

        #horizontal weights
        for i in range(H):
            for j in range(W - 1):
                wfile.write(C[1 + j+ i*W][1 + j+1 + i*W]+ " ")
            wfile.write("\n")

        #vertical weights
        for i in range(H - 1):
            for j in range(W):
                wfile.write(C[1 + j +  i*W][1 + j + W + i*W]+ " ")
            wfile.write("\n")


def DIMACStoMAT(srcfile, dstfile):
    with open(srcfile, "r") as rfile:
        line_list = rfile.readline().split(" ")
        if(line_list[0] == 'p'):
            vertices = (int)(line_list[2])
            print (line_list[2] + "\n")
            source = [0 for i in range(vertices - 2)]
            dest = [0 for i in range(vertices - 2)]
        line = rfile.readline()
        while (not line.startswith("c")):
            line = rfile.readline()
        line_list = [x.strip() for x in line.split()]
        #print (line_list)
        if(line_list[0] == 'c' and line_list[1] == 'regulargrid'):
            w , h = (int)(line_list[2]),(int)(line_list[3])
            horizontal = [[0 for i in range(w-1)] for j in range(h)]
            vertical = [[0 for i in range(w)] for j in range(h-1)]
            #print (C)
        #print(horizontal)
        for line in rfile:
            if(line.startswith("a")):
                s,d,c = [(int)(x.strip()) for x in line[1:].split()]
                #print ( s,d,c)
                if(s == 1):
                    source[d - 3] = c
                elif(d == 2):
                    dest[s - 3] = c
                elif((d - s) == 1):
                    horizontal[math.floor((s - 3)/ w)][(s - 3)% w] = c
                elif((d - s) == w):
                    vertical[math.floor((s - 3)/ w)][(s - 3)% w] = c
        
        with open(dstfile, "w") as wfile:
            wfile.write(str(w) + " " + str(h) + " " + "2\n")
            
            #dummy output
            for i in range(h):
                for j in range(w):
                    wfile.write(str(i) + " ")
                wfile.write("\n")
            
             #sink and target weights
            for i in range(h):
                for j in range(w):
                    wfile.write(str(source[j + i*w])+ " ")
                wfile.write("\n")

            for i in range(h):
                for j in range(w):
                    wfile.write(str(dest[j + i*w])+ " ")
                wfile.write("\n")

            #horizontal weights
            for i in range(h):
                for j in range(w - 1):
                    wfile.write(str(horizontal[i][j])+ " ")
                wfile.write("\n")

            #vertical weights
            for i in range(h - 1):
                for j in range(w):
                    wfile.write(str(vertical[i][j])+ " ")
                wfile.write("\n")

            
    


def covertDIMACSToCuts(srcfile, dstfile):
    DIMACStoMAT(srcfile,dstfile)


def main(argv):
    if len(argv) > 2:
        covertDIMACSToCuts(argv[1], argv[2])


if __name__ == "__main__":
    main(sys.argv)