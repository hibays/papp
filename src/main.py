import os
import sys
import json
from typing import Dict

import launcher as lun # type: ignore
from launcher import __launcher_dir__, __runtime_dir__ # type: ignore

from pathlib import Path

def getWinDrive(path) :
	try :
		return path.split(':')[0] + ':'
	except : # noqa: E722
		raise ValueError(f'Unresolvable drive path: {path}')
	
def stdlizePath(path: Path) :
	ls_parts = path.parts
	nc = Path(ls_parts[0])
	for p in ls_parts[1:] :
		nc /= p
	return nc

def exangeUser(data_dir) : # Change env vars method
	# Ref: https://www.coder.work/article/975869
	absp = stdlizePath(Path(data_dir)).absolute().__str__()
	lun.setenv('USERPROFILE', absp)
	lun.setenv('HOMEDRIVE', getWinDrive(absp))
	lun.setenv('HOMEPATH', absp.lstrip(getWinDrive(absp)))
	lun.setenv('APPDATA', Path(absp).joinpath('AppData').joinpath('Roaming').__str__())
	lun.setenv('LOCALAPPDATA', Path(absp).joinpath('AppData').joinpath('Local').__str__())

def tranEnv(path_str: str)-> str :
	ns = path_str.replace('{Path}', str(os.getenv('Path'))
		).replace('{AppData}', str(os.getenv('APPDATA'))
		).replace('{USERPROFILE}', str(os.getenv('USERPROFILE')),
		).replace('{runtime}', __runtime_dir__)
	
	return stdlizePath(Path(ns)).__str__()

class exangeDirMgr(object) :#TODO: A way to map folders
	def __init__(self) :
		...

	def make_map(self, target, link) :
		...

	def __enter__(self):
		self.mapPath = []
		return self

	def __exit__(self, type, value, trace):
		while self.mapPath :
			target, link = self.mapPath.pop()
			os.remove(link)

def load_config(json_path: Path) -> Dict:
	configd: Dict = json.loads(json_path.read_text())

	configd['data_dir'] = tranEnv(configd['data_dir'])

	configd['executable_path'] = tranEnv(configd['executable_path'])

	return configd

def main() :
	conf = load_config(Path(__runtime_dir__) / 'pconf.json')
	print(conf)

	if conf['portable_mode'] == 'env' :
		lun.create_directories(conf['data_dir'])
		exangeUser(conf['data_dir'])

	elif conf['portable_mode'] == 'map' :
		raise NotImplementedError('Not support `map` mode yet.') #TODO: `map` mode
		for i in conf['mapPath'] :
			target, link = i

	else :
		raise ValueError('Invaild config for portable_mode, must be `env` or `map`.')
	
	for k,v in conf['ext_env'].items() :
		lun.setenv(k, tranEnv(v))

	# Enter RT env
	if os.curdir == __launcher_dir__ :
		print('-- Changing working directory to target app dir:', os.path.dirname(conf['executable_path']))
		os.chdir(os.path.dirname(conf['executable_path']))

	runas: bool = conf['RunAsAdministrator'].lower() == 'yes'

	exec_path: str = conf['executable_path']
	exec1_args = ' '.join(sys.argv[1:])
	exec0_args = sys.argv[0] + '{}'.format(' ' + exec1_args if exec1_args else '')

	print('-- Constructing command:', exec_path, f'(:{sys.argv[0]})')

	if exec_path == lun.__exec_path__ :
		# If the target app is launcher itself, use stimu_pipe_run
		if runas :
			raise ValueError('Cannot run launcher as administrator.')
		else :
			lun.msgbox('Warning', 'You are running the launcher itself, this may cause unexpected behavior.')

	if runas :
		conclog = lun.shell_exec(exec_path + ' ' + exec1_args, runas)
	else :
		conclog = lun.stimu_pipe_run(exec_path, exec0_args)
	print('conclog:', conclog)


if __name__ == '__main__' :
	print(sys.argv)
	try :
		main()

	except Exception as e:
		lun.msgbox(type(e).__name__, e.__str__())
		raise e