import random

filename = raw_input("Enter file name: ")
ofile = open(filename, 'w')
ncommands = input("Enter number of commands: ")

commands = [ chr(27)+'[A', chr(27)+'[B', chr(27)+'[C', chr(27)+'[D', chr(27)+'[F', chr(27)+'[H', chr(27)+'[3~', chr(27)+'[5~', chr(27)+'[6~', chr(1), chr(5), chr(9), chr(10), chr(127) ]

characters = []

for i in range(32, 127):
	characters.append(chr(i))

#print(commands)
#print(characters)

for i in range(0, ncommands):
	for j in range(0, 21):
		temp = random.randint(1,100000) % 101 
		if(temp >= 60):
			char = random.choice(characters)
			ofile.write(char)
		else:
			comm = random.choice(commands)
			ofile.write(comm)
		if(j == 20):
			ofile.write('\n')
