import os;
from subprocess import Popen, PIPE;

# Get all files (recursively) under folder direc ending with ext(ension)
def get_list_of_files(direc, ext=""):
	files = list() ## a list of all files
	dir_content = os.listdir(direc)

	for c in dir_content:
		path = os.path.join(direc, c)
		if os.path.isdir(path): ## check if it is a directory
			## call the function in the directory to get its files
			files += get_list_of_files(path, ext)
		elif not ext or c.endswith(ext):
			files.append(path)
		
	return files

				
if __name__ == '__main__': 
    #read parameters
	cdir = "../configurations/" ## directory with all configuration files
	ext = ".txt"
	idir = "../instances/"

	if not os.path.exists(cdir):
		print "Missing configurations folder ('../configurations/'). Did you forget to create it?"
	if not os.path.exists(idir):
		os.makedirs(idir)
	
	for filename in get_list_of_files(cdir, ext):
		ifn = "%s%s" % (idir, filename.split("/")[-1])
		ag1 = "-c%s" % (filename)
		ag2 = "-o%s" % (ifn)
		process = Popen(["../main", ag1, ag2], stdout=PIPE)
		(output, err) = process.communicate()
		exit_code = process.wait()
		if exit_code != 0:
			print output,
			print "Error generating %s" % (filename)
		else:
			print "Generated %s" % (filename.split("/")[-1])

	print "DONE"
