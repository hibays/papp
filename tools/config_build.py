from os import rename, system, chdir
from pathlib import Path
from shutil import rmtree

import tkinter as tk
from tkinter import filedialog

def run_cmake_build() :
	build_dir = Path('build')
	build_dir.mkdir(exist_ok=True)
	chdir(build_dir)
	system('cmake ..')
	system('cmake --build .')
	chdir('..')

def main(app_name=None, *, icon=None) :
	build_dir = Path('build')
	rmtree(build_dir, ignore_errors=True)
	
	if app_name is None :
		app_name = input('Enter your PortableApp name: ')

	if icon is None : # TODO
		if (input('Choose a icon? [Y/n] ')+'y')[0] not in 'Nn' :
			root = tk.Tk()
			root.withdraw()
			f_path = filedialog.askopenfilename()

		else :
			print('Using default icon.')

	run_cmake_build()

	Path('PAPP.exe').rename('%s.exe' % app_name)

if __name__ == '__main__' :
	main()