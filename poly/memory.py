import math
import sys
import re

class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    def __str__(self):
        return f"{(self.x, self.y)}"
    def __add__(self, other):
        return Point(self.x+other.x, self.y+other.y)
    def to_tikz(self):
        return str(self)
    
class Label():
    def __init__(self, p, texte, options=None):
        self.p = p
        self.texte = texte
        self.options = options

    def to_tikz(self):
        opts = "" if not self.options else f"[{self.options}]"
        return f"\\node{opts} at {self.p} {{{self.texte}}};\n"

class Path():
    def __init__(self, *points, color=None, bgcolor=None, cycle=None, style=None):
        self.points = []
        for p in points:
            q = p if isinstance(p, Point) else Point(*p)
            self.points.append(q)
        #self.points = [Point(q) for q in p]
        self.color=color
        self.bgcolor=bgcolor
        self.cycle = cycle
        self.style = style
        self.sucs = []
    def to_tikz(self):
        s =""
        path = " -- ".join( (p.to_tikz() for p in self.points))
        if self.cycle:
            path += " -- cycle"
        if self.bgcolor:
            s = f"\\fill[color={self.bgcolor}] {path};\n" if self.bgcolor else ""
        style = self.style + "," if self.style else ""        
        if self.color:
            s += f"\\draw[{style}color={self.color}] {path};\n"
        s += "\n".join((s.to_tikz() for s in self.sucs))
        return s

class Rectangle(Path):
    def __init__(self, p1, p2, color=None, bgcolor=None):
        super().__init__(p1, Point(p2.x, p1.y), p2, Point(p1.x,p2.y), 
            color=color, bgcolor=bgcolor, cycle=True)
    @property
    def x(self):
        return self.points[0].x
    @property
    def y(self):
        return self.points[0].y

class Memory(Rectangle):
    def __init__(self, L, l, y=0):
        super().__init__(Point(0,y),Point(L,y+l),bgcolor=None,color="black")
        self.l = l
        self.m = []
        self.data = []

    def to_tikz(self):
        s= super().to_tikz()
        for pos, label, h, above in self.m:
            if above:
                S = Point(pos,self.y+self.l+h)
                T = Point(pos, self.y+self.l)
                rel = "above"
            else:
                S = Point(pos, self.y-h)
                T = Point(pos, self.y)
                rel = "below"
            
            s += f"\\draw[->] {S.to_tikz()} -- {T.to_tikz()};\n"
            s += f"\\node[{rel}] at {str(S)} {{{label}}};\n"
        for d in self.data:
            s += d.to_tikz()
        return s

    def mark(self, pos, label, h=0.5, above = False):
        """
        add a mark at position pos, with label
        h=0.5 is the default height of the mark,
        above is the relative position
        """
        self.m.append((pos, label, h, above))
    def content(self, pos, label,draw_bar=1):
        self.sucs.append(Path(Point(pos,self.y), Point(pos,self.y+self.l),color="black"))
        self.sucs.append(Label(Point(pos, self.y+self.l/2), label, options="right"))

    def add_node(self,pos,label):
        self.sucs.append(Label(Point(pos[0],pos[1]), label))
    def bar(self, *pos):
        for p in pos:
            self.content(p, "")
    def zone(self, x1,x2,bgcolor=None, color=None):
        self.sucs.append(Rectangle(Point(x1,self.y),Point(x2,self.y+self.l),color=color,bgcolor=bgcolor))

def draw(*liste):
    return "\n".join([m.to_tikz() for m in liste])
    
if __name__ == "__main__":
    with open(f"{sys.argv[1]}/{sys.argv[2]}.mem") as f:
        data = f.read()
    before, after, drawing = [], [], []

    for line in data.split("\n"):
        if _pattern_match := re.match("#BEFORE\s*:\s*(.*)", line):
            before.append(_pattern_match.group(1))
        elif _pattern_match := re.match("#AFTER\s*:\s*(.*)", line):
            after.append(_pattern_match.group(1))
        elif "draw" in line:
            drawing.append(line)
    exec(data)
    if not drawing:
        drawing = ["draw(m)"]
    body = "\n".join((eval(line) for line in drawing)) 
    head = "\\begin{tikzpicture}\n" + '\n'.join(before)
    foot = "\n".join(after) + "\n\\end{tikzpicture}"
    s = head + body + foot
    with open(f"{sys.argv[1]}/{sys.argv[2]}.tex", "w") as f:
        f.write(s)
    print(s)
