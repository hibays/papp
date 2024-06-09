__exec_path__: str
__launcher_dir__: str
__runtime_dir__: str

def setenv(name: str, value: str)-> int :
	...

def stimu_pipe_run(app_abs_path: str, app_args: str)-> str :
	...

def shell_exec(cmd: str, runas: bool=False)-> bool :
	...

def msgbox(title: str, text: str)-> int :
	...
