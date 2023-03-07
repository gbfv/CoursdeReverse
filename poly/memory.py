import math
import sys
import re

class Zone():
    def __init__(self, x1,x2,y1,y2,color):
        self.x1 = x1
        self.x2 = x2
        self.color = color
        self.y1 = y1
        self.y2 = y2
    def to_tikz(self):
        return f"\\fill[color={self.color}] {(self.x1,self.y1)} -- {(self.x2,self.y1)} -- {(self.x2,self.y2)} --{(self.x1,self.y2)};\n"

class Label():
    def __init__(self, x, y, texte):
        self.x = x
        self.y = y
        self.texte = texte

    def to_tikz(self):
        return f"\\node at {(self.x,self.y)} {{{self.texte}}};\n"

class Content():
    def __init__(self, x, y, texte,draw_bar):
        self.x = x
        self.y = y
        self.texte = texte
        self.draw_bar = draw_bar
    def to_tikz(self):
        s = f"\\draw ({self.x}, 0) -- {(self.x, self.y)};\n" if self.draw_bar else ""
        return s+f"\\node[right] at {(self.x,self.y/2)} {{{self.texte}}};\n"

class Node():
    def __init__(self, x, y, texte):
        self.x = x
        self.y = y
        self.texte = texte
    def to_tikz(self):
        return f"\\node at {(self.x,self.y)} {{{self.texte}}};\n"
        

class Memory():
    def __init__(self, L, l):
        self.L = L
        self.l = l 
        self.m = []
        self.data = []

    def to_tikz(self, before, after):
        s = "\\begin{tikzpicture}\n"
        s += "\n".join(before)
        for pos, label, h, above in self.m:
            if above:
                s += f"\\draw[->] ({pos}, {self.l+h}) -- ({pos}, {self.l});\n"
                s += f"\\node[above] at ({pos}, {self.l+h}) {{{label}}};\n"
            else:
                s += f"\\draw[->] ({pos}, {-h}) -- ({pos}, 0);\n"
                s += f"\\node[below] at ({pos}, {-h}) {{{label}}};\n"

        for d in self.data:
            s += d.to_tikz()
        s += "\n".join(after)
        s += f"\n\\draw (0,0) -- ({self.L},0) -- ({self.L}, {self.l}) -- (0, {self.l}) -- cycle;\n"
        s += "\n\\end{tikzpicture}"
        return s

    def mark(self, pos, label, h=0.5, above = False):
        self.m.append((pos, label, h, above))
    def content(self, pos, label,draw_bar=1):
        self.data.append(Content(pos, self.l, label,draw_bar))
    def add_node(self,pos,label):
        self.data.append(Node(pos[0],pos[1], label))
    def bar(self, *pos):
        for p in pos:
            self.content(p, "")
    def zone(self, x1,x2,color):
        self.data.append(Zone(x1,x2,0,self.l,color)) 

if __name__ == "__main__":
    with open(f"{sys.argv[1]}/{sys.argv[2]}.mem") as f:
        data = f.read().split("\n")
    before = []
    after = []
    for line in data:
        if _pattern_match := re.match("#BEFORE\s*:\s*(.*)", line):
            before.append(_pattern_match.group(1))
        elif _pattern_match := re.match("#AFTER\s*:\s*(.*)", line):
            after.append(_pattern_match.group(1))
        else:
            exec(line)
    s = eval(f"m.to_tikz(before, after)")
    with open(f"{sys.argv[1]}/{sys.argv[2]}.tex", "w") as f:
        f.write(s)
    print(s)

