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

def exangeUser(data_dir) : # Change env vars method
	# Ref: https://www.coder.work/article/975869
	absp = Path(data_dir).absolute().__str__()
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
	
	new_conc = Path(ns).parts
	nc = Path(new_conc[0])
	for p in new_conc[1:] :
		nc /= p
	return nc.__str__()

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

	configd['data_dir'] = Path(tranEnv(configd['data_dir'])).absolute().__str__()

	configd['executable_path'] = tranEnv(configd['executable_path'])

	return configd

def main() :
	cod = load_config(Path(__runtime_dir__) / 'pconf.json')
	print(cod)

	if cod['appReDataMode'] == 'env' :
		lun.create_directories(cod['data_dir'])
		exangeUser(cod['data_dir'])

	elif cod['appReDataMode'] == 'map' :
		raise NotImplementedError('Not support `map` mode yet.') #TODO: `map` mode
		for i in cod['mapPath'] :
			target, link = i

	else :
		raise ValueError('Invaild config for appReDataMode, must be `env` or `map`.')
	
	for i in cod['envVars'] :
		lun.setenv(i[0], i[1])

	# Enter RT env
	if Path(os.curdir) == Path(__launcher_dir__) :
		os.chdir(os.path.dirname(cod['executable_path']))

	runas: bool = cod['RunAsAdministrator'].lower() == 'yes'

	exec_path = cod['executable_path']
	exec0_args = ''
	for i in sys.argv :
		exec0_args += f' "{i}"'

	if False :
		print('runas', runas)
		# If the app is launcher itself, use stimu_pipe_run
		if runas :
			raise ValueError('Cannot run launcher as administrator.')
		else :
			lun.msgbox('Warning', 'You are running the launcher itself, this may cause unexpected behavior.')

	if runas :
		endr = lun.shell_exec(' '.join((exec_path, exec0_args)), runas)
	else :
		endr = lun.stimu_pipe_run(exec_path, exec0_args)
	print('endr', endr)

if __name__ == '__main__' :
	print(sys.argv)
	try :
		main()

	except Exception as e:
		lun.msgbox(type(e).__name__, e.__str__())
		raise e