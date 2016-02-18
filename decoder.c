#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL 26

// CIS 361: Project 1
// decoder.c
// Caesar Cipher Decoder
// Chad Mersino
// Assisting code provided by Professor Yonglei Tao

void calcFreq(float found[], char fname[]);
// Added a char array to specify for the file writer
// where the output should end up
void decrypt(int key, char fname[],char out[]);
char encrypt(char ch, int k);
int findKey(float given[], float found[]);
void readFreq(float given[], char fname[]);
// Contains different parameters than expected, explained over
// method itself below
void rotate(float given[]);



// Read the encoded text from an input file and accumulate the letter frequency
// data for the encoded text. Store the frequency data in array found.
void calcFreq(float found[], char fname[]){
      FILE *folder;
      folder = fopen(fname, "r");
      int x;
      int sum = 0;
      if(folder == NULL){
		printf("Error: Could not open encrypted file data.out");
		exit(0);
	}	
	
	// Set all spots in array to 0 for functionality later on
	// Unfortunately cannot use found[] = {0} because this array
	// is from a parameter
	for(x = 0; x < TOTAL; x++){
		found[x] = 0;
	}

	// Count number of letter frequency for each letter in alphabet
	char temp = fgetc(folder);
	// Keep going until end of the file
	while(temp != EOF){
		// If there is a letter
		if(isalpha(temp)){
			sum++;
			// To upper to take care of both lower and upper
			temp = toupper(temp);
			found[temp - 'A']++;
		}
		// Set temp from folder stream and moves character
		// from that stream
		temp = fgetc(folder);
	}

	// Based off file, update letter frequencies
	for(x = 0; x < TOTAL; x++){
		found[x] = (found[x] / sum);
	}
}

// Decrypt the encoded text in the input file using the key and display the decoded text
void decrypt(int key, char fname[],char out[]){
  //Read file and use rotate(key) on each char to get the message

        FILE *outputFile, *inputFile;
    	inputFile = fopen(fname, "r");
    	outputFile = fopen(out, "w");
    	char temp = fgetc(inputFile);
	
	// No key? Set to this value
    	if (key < 0){
            key = key + 26;
	}

    	while(temp != EOF){
	   // Prints everything to the output file to receive the correct output
    	   if(isupper(temp)){
    		fprintf(outputFile, "%c", (temp - key + TOTAL - 'A') %TOTAL + 'A');
	   }
           else  if (islower(temp)){
                fprintf(outputFile, "%c", (temp - key + TOTAL - 'a') %TOTAL + 'a');
	   }
           else{
		fprintf(outputFile, "%c", temp);
	   }
	// Set temp from input file stream and moves character
	// from that stream
    	temp = fgetc(inputFile);
    	}
}

// Encryption method received from cipher.c
// Used as a fail safe and for ease of development to
// keep everything to one file
char encrypt(char ch, int k){
	
	if (k < 0){
	  k = k + 26;
	}
	if (isupper(ch)){
	  return (ch - 'A' + k) % 26 + 'A';
	}
	if (islower(ch)){
	  return (ch - 'a' + k) % 26 + 'a';
	}
	return ch;
}


// Compare given to found and find the key based off those arrays.
// Go through each rotation possible, which is done based off float
// values in the given array instead of going by an individual character
// and the key that should go with it
int findKey(float given[], float found[]){

	int x;
	int y;
	float temp;
	int key = 0;
	int initial = 1;
	float sum = 0;
	float average;

	// Loop through alphabet
	for(y = 0; y < TOTAL; y++){
		// This needs to be reset on each pass
		sum = 0;

		//sums the differences between the arrays
		for(x = 0; x < TOTAL; x++){
		  temp = found[x] - given[x];
		  temp = temp * temp;
		  sum += temp;
		}
		
		// If there needs to be a more valid number,
		// just set it to average and the key to
		// as far as the loop went
		if(sum < average){
		  average = sum;
		  key = y;
		}
		
		// If it is the first pass through
		if(initial){
		  average = sum;
		  initial = 0;
		}
		
		// Instead of roatating character by character,
		// I rotate based off the given values for the float values
		// from given, which changes the parameters of the function call
		// Seems to simplify the process a little bit
		rotate(given);
	}

	return key;
}

// Receives frequencies for a given letter based off the LetFreq.txt file
void readFreq(float *given, char fname[]){
	FILE *folder;
	folder = fopen(fname, "r");
	char buf[1024];
  	int cnt = 1;
	int x = 0;

  	if(folder == NULL){
	  printf("Error:  Could not open LetFreq.txt!!\n");
	  exit(0);
  	}
  	
  	// Tokenize every line in buffer, retrieve numerical values
  	while((fgets(buf, 1024, folder)) != NULL){
  		char *tokenStr;
  		tokenStr = strtok(buf, "\n ");
  		tokenStr = strtok(NULL, "\n ");
  		given[x] = atof(tokenStr);
  		x++;
  	}
}

// This method uses the floats that were given from findKey method.
// Instead of going based off of individual characters and the key,
// I take the float values from given[] and rotate based off that
void rotate(float given[]){

        int x;
        float temp;
	// This gets the last value inside the given float array
	float lastL = given[TOTAL-1];

	// Move from end to begninning of array, pushes
	// up the array by one spot every time
        for(x = 25; x >= 0; x--){
	  temp = given[x-1];
          given[x] = temp;
        }
        
        // Set the first spot to the last letter in alphabet
        given[0] = lastL;
	
}

// Main method for decoder.c
int main(int argc, char* argv[]){
  
	FILE *fin;
	FILE *fout;
	int option;
	int key;
	char ch;
	// Applies all spots to 0
	float given[26] = {0};
	float found[26] = {0};

	// Initial check for correct arguments, take from cipher.c
	if (argc != 5){
		printf ("Parameters: cipher option, key, input file, output file\n");
		printf ("Cipher option 1 : encryption\tCipher option 2 : decryption\n");
		exit(1);
	}

	// Assign integer values to key and selection of encryption/decription
	option = atoi(argv[1]);
	key = atoi(argv[2]);
	
	// This invokes decription when user prompts input option as 2
	if (option == 2){
	  // Process for decoding:
	  // 1.Read file 2.Calcuate frequencies given
	  // 3.Find the key for decoding and 4.Decode the file 
		readFreq(given, "LetFreq.txt");
		calcFreq(found, argv[3]);
		int key = findKey(given, found);
		decrypt(key, argv[3], argv[4]);
	}
	
	// This was made as a fail safe to using the makefile, it will
	// encrypt regardless of if it was before when the user wishes to do
	// such. Also this made developing much easier because I only needed
	// to run my file for it to execute the full encrypt/decrypt process
	else{
		// Main files to receive code to encrypt and file to send output to
		fin = fopen(argv[3], "r");
		fout = fopen(argv[4], "w");

		// Checks if files exist
		if (fin ==  NULL || fout == NULL){
			printf("File could not be opened\n");
			exit(1);
		}

		// Prints encrypted text to the output file defined above
		while (fscanf(fin, "%c", &ch) != EOF){
			fprintf(fout, "%c", encrypt(ch, key));
		}

		// Seal off files after being written to or read from
		fclose(fin);
		fclose(fout);
	}

	return 0;
}


