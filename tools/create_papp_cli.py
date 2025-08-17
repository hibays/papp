from os import system, chdir
from pathlib import Path
from shutil import rmtree
from subprocess import Popen

from zipfile import ZipFile


class CmakeBuilder(object):

    def __init__(self,
                 src_dir: Path = Path(__file__).parent.parent,
                 build_dir: Path = Path('build')):
        self.src_dir: Path = src_dir
        self.build_dir: Path = build_dir
        self.build_dir.mkdir(exist_ok=True)

    def clean(self):
        rmtree(self.build_dir, ignore_errors=True)

    def run_configure(self):
        chdir(self.build_dir)
        system('cmake %r' % self.src_dir)
        chdir(self.src_dir)

    def run_build(self):
        chdir(self.build_dir)
        system('cmake --build .')
        chdir(self.src_dir)


class ConfigMgr(object):

    def __init__(self):
        self.configs: dict = {}
        self.init_default()

    def init_default(self):
        self.configs = {
            'portable_mode': 'env',
            'data_dir': 'data',
            'work_dir_fallback': '.',
            'work_dir_force_fallback': False,
            'runas': False,
            'entry_args': [],
            'ext_env': {},
        }


class PappBuilder(object):

    def __init__(self, app_name: Path, build_dir: Path):
        self.app_name: Path = app_name
        self.build_dir: Path = build_dir
        self.build_dir.mkdir(exist_ok=True)


@click.command()
@click.argument('target_exe', type=click.Path(exists=True))
def main(target_exe,
         *,
         app_name: str = None,
         icon: Path = None,
         build_dir: Path = None,
         data_dir: Path = None):
    '''Create PortableApp

	target_exe: Path
	目标可执行文件


	'''

    ...


if __name__ == '__main__':
    main()
