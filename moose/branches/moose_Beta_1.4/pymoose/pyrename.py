import os
for filename in os.listdir('.'):
    if filename.endswith('.cpp') and not filename.startswith('py') and not filename.startswith('Py'):
        os.rename(filename, 'py'+filename)
print "Prefixed all .cpp files with 'py'"        
        
        
