#include "ucode.c"

int checkSub(char * sub, char * line)
{
	//run loop until line empty
	while(*line != '\0'){
		//if found something similar
		if(*line == *sub){
			//check if whole substring is the same and return 1, check next until return 0
			for(char * sub2 = sub; *line != '\0'; sub2++, line++){
				if(*sub2 == '\0'){
					return 1;
				}
				else if(*sub2 != *line){
					break;
				}
			}
		}
		line++;
	}
	return 0;
}

int main(int argc, char * argv[])
{
	char buf[1024];
	
	if(argc > 2)
	{
		close(0);
		//if input correct, open file for read
		open(argv[2], O_RDONLY);
	}

	if(argc < 2)
	{
		prints("Usage: grep pattern [file]\n");
		return 0;
	}
	
	//get each line and run it through check sub
	while(getline(buf)){
		if(checkSub(argv[1], buf) == 1){
			//if found, print whole line
			prints(buf);
		}
	}
	return 0;
}
