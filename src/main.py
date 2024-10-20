import os
import sys
import json

import launcher as lun # type: ignore
from launcher import __launcher_dir__, __runtime_dir__ # type: ignore

def getWinDrive(path) :
	try :
		return path.split(':')[0] + ':'
	except : # noqa: E722
		raise ValueError(f'Unresolvable drive path: {path}')

def exangeUser(poraDataDir) :
	# https://www.coder.work/article/975869
	absp = os.path.abspath(poraDataDir)
	lun.setenv('USERPROFILE', absp)
	lun.setenv('HOMEDRIVE', getWinDrive(absp))
	lun.setenv('HOMEPATH', absp.lstrip(getWinDrive(absp)))
	lun.setenv('APPDATA', os.path.join(absp, 'AppData', 'Roaming'))
	lun.setenv('LOCALAPPDATA', os.path.join(absp, 'AppData', 'Local'))

def tranEnv(path_str: str)-> str :
	ns = path_str.replace('{Path}', str(os.getenv('Path'))
		).replace('{AppData}', str(os.getenv('APPDATA'))
		).replace('{USERPROFILE}', str(os.getenv('USERPROFILE')),
		).replace('{runtime}', __runtime_dir__)
	
	return ns

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

def load_config(jsonF) :
	with open(jsonF) as f :
		configd: dict = json.loads(f.read())

	configd['poraDataDir'] = os.path.abspath(tranEnv(configd['poraDataDir']))

	configd['appExecAbs'] = tranEnv(configd['appExecAbs'])

	return configd

def main() :
	cod = load_config(os.path.join(__runtime_dir__, 'papp_config.json'))

	if cod['appReDataMode'] == 'env' :
		lun.create_directories(cod['poraDataDir'])
		exangeUser(cod['poraDataDir'])

	elif cod['appReDataMode'] == 'map' :
		raise NotImplementedError('Not support `map` mode yet.') #TODO: `map` mode
		for i in cod['mapPath'] :
			target, link = i

	else :
		raise ValueError('Invaild config for appReDataMode, must be `env` or `map`.')
	
	for i in cod['envVars'] :
		lun.setenv(i[0], i[1])

	# Enter RT env
	if os.path.abspath(os.curdir) == __launcher_dir__ :
		os.chdir(os.path.dirname(cod['appExecAbs']))

	runas: bool = cod['RunAsAdministrator']

	if runas :
		endr = lun.shell_exec(' '.join((cod['appExecAbs'], *map('"{}"'.format, sys.argv))), runas) # type: ignore
	else :
		endr = lun.stimu_pipe_run(cod['appExecAbs'], ' '.join((lun.__exec_path__, *map('"{}"'.format, sys.argv)))) # type: ignore
	print(endr)

if __name__ == '__main__' :
	print(sys.argv)
	try :
		main()

	except Exception as e:
		lun.msgbox(e.__class__.__name__, e.__str__())
		raise e