import os
import sys
import json
from typing import Dict

import launcher as lun # type: ignore
from launcher import __launcher_dir__, __runtime_dir__ # type: ignore

from pathlib import Path

def stdlizePath(path: Path) :
	# format path to native status
	ls_parts = path.parts
	nc = Path(ls_parts[0])
	for p in ls_parts[1:] :
		if p == '/' :
			p = '\\'
		nc /= p
	return nc

def conf_embed(path_str: str)-> str :
	return (path_str
		).replace('#p{Path}', str(os.getenv('Path'))
		).replace('#p{AppData}', str(os.getenv('APPDATA'))
		).replace('#p{USERPROFILE}', str(os.getenv('USERPROFILE')),
		).replace('#p{runtime}', __runtime_dir__)

def load_config(json_path: Path) -> Dict:
	configd: Dict = json.loads(json_path.read_text())

	# stdlize bool
	for i in ['runas', 'work_dir_force_fallback'] :
		if configd[i] not in {'yes', 'no'} :
			raise ValueError(f'Config item `{i}` must be `yes` or `no`.')
		configd[i] = configd[i].lower() == 'yes'
	
	# stdlize paths
	for i in ['data_dir', 'executable_path', 'work_dir_fallback'] :
		if i not in configd :
			raise KeyError(f'Missing key `{i}` in config file.')
		configd[i] = stdlizePath(Path(conf_embed(configd[i]))).__str__()

	# stdlize entry args
	configd['entry_args'] = list(map(conf_embed, configd['entry_args']))

	# stdlize ext env
	for k,v in configd['ext_env'].items() :
		configd['ext_env'][k] = conf_embed(v)

	# stdlize portable mode
	if configd['portable_mode'] not in {'map', 'env'} :
		raise ValueError('`portable_mode` must be `map` or `env`.')
	
	# stdlize map mode path
	if configd['portable_mode'] == 'map' :
		if 'mapPath' not in configd :
			raise KeyError('Missing key `mapPath` in config file.')
		if type(configd['mapPath']) != list :
			raise ValueError('`mapPath` must be a list.')
		for idx, i in enumerate(configd['mapPath']) :
			if type(i) != list or len(i) != 2 :
				raise ValueError(f'`mapPath` item at index {idx} must be a list with 2 elements.')
			target, link = i
			configd['mapPath'][idx] = [
				stdlizePath(Path(conf_embed(target))).__str__(),
				stdlizePath(Path(conf_embed(link))).__str__()
			]

	return configd


class overrideEnvMgr(object) :
	def __init__(self, data_dir: Path) :
		if type(data_dir) != Path :
			raise ValueError('`data_dir` must be a `Path` object.')
		self.data_dir = data_dir

	def do_override(self) :
		# Ref: https://www.coder.work/article/975869
		absp = self.data_dir.absolute()
		absps = absp.__str__()
		lun.setenv('USERPROFILE', absps)
		lun.setenv('HOMEDRIVE', absp.drive)
		lun.setenv('HOMEPATH', absps.lstrip(absp.drive))
		lun.setenv('APPDATA', (absp / 'AppData' / 'Roaming').__str__())
		lun.setenv('LOCALAPPDATA', (absp / 'AppData' / 'Local').__str__())


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


class AppLauncher(object) :
	def __init__(self, conf: Dict) :
		self.conf = conf

	def launch(self) :
		...


def main() :
	conf = load_config(Path(__runtime_dir__) / 'pconf.json')

	if conf['portable_mode'] == 'env' :
		lun.create_directories(conf['data_dir'])
		overrideEnvMgr(Path(conf['data_dir'])).do_override()

	elif conf['portable_mode'] == 'map' :
		raise NotImplementedError('Not support `map` mode yet.') #TODO: `map` mode
		for i in conf['mapPath'] :
			target, link = i

	else :
		raise ValueError('Invaild config for portable_mode, must be `env` or `map`.')
	
	for k,v in conf['ext_env'].items() :
		lun.setenv(k, v)

	# Enter RT env
	if os.curdir == __launcher_dir__ :
		newcwd: str = os.path.dirname(conf['executable_path'])
		if conf['work_dir_force_fallback'] :
			newcwd: str = conf['work_dir_fallback']
			print('-- Warning: Work dir force fallback enabled, using fallback dir:', conf['work_dir_fallback'])
		elif not Path(newcwd).is_dir() :
			print('-- Warning: Target app dir not found, using fallback dir:', conf['work_dir_fallback'])
			newcwd: str = conf['work_dir_fallback']
		print('-- Changing working directory to target app dir:', newcwd)
		os.chdir(newcwd)

	# Insert entry args
	for arg in conf['entry_args'][::-1] :
		sys.argv.insert(1, arg)

	print('-- Resolved argv (include argv[0]):', sys.argv)

	runas: bool = conf['runas']

	exec_path: str = conf['executable_path']
	exec1_args = ' '.join(sys.argv[1:])
	execp_args = sys.argv[0] + '{}'.format(' ' + exec1_args if exec1_args else '')
	execo_args = exec_path + '{}'.format(' ' + exec1_args if exec1_args else '')

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
		conclog = lun.stimu_pipe_run(exec_path, execo_args)
	print('conclog:', conclog)


if __name__ == '__main__' :
	try :
		main()

	except Exception as e:
		lun.msgbox('Papp: ' + type(e).__name__, e.__str__())
		raise e