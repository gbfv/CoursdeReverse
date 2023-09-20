
char fonction[]= {'\x90','\x90','\x90','\xc3'};

typedef void (*typeFonction) (); 
int main(){
        typeFonction mf = (typeFonction) &fonction;
        mf();
        printf("done");
	...
}

