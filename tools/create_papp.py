from os import system, chdir
from pathlib import Path
from shutil import rmtree
from subprocess import Popen

import tkinter as tk
from tkinter import filedialog
from tkinter import messagebox


class CmakeBuilder(object) :
	def __init__(self, src_dir: Path=Path(__file__).parent.parent, build_dir: Path=Path('build')) :
		self.src_dir: Path = src_dir
		self.build_dir: Path = build_dir
		self.build_dir.mkdir(exist_ok=True)

	def clean(self) :
		rmtree(self.build_dir, ignore_errors=True)

	def run_configure(self) :
		chdir(self.build_dir)
		system('cmake %r' % self.src_dir)
		chdir(self.src_dir)

	def run_build(self) :
		chdir(self.build_dir)
		system('cmake --build .')
		chdir(self.src_dir)


class IconMgr(object) :
	def __init__(self, icon_src: Path) :
		self.icon_src: Path = icon_src

	def tk_select_icon(self, root: tk.Tk) -> Path:
		"""
		弹出文件选择对话框，让用户选择一个图标文件（.ico）。
		如果选择无效或取消，则循环重试，直到选择有效文件或用户退出。
		返回选择的 Path 对象，若用户取消则返回 None。
		"""
		supported_extensions = {'.ico'}  # 可扩展支持 .png/.icns 等，但 Windows 通常用 .ico

		while True:
			f_path = filedialog.askopenfilename(
				title="请选择一个图标文件 (.ico)",
				filetypes=[
					("Icon Files", "*.ico"),
					("All Files", "*.*")
				],
				parent=root
			)

			if not f_path:  # 用户点击取消或关闭对话框
				if messagebox.askyesno("提示", "未选择图标文件，是否取消并使用默认图标？"):
					return None # type: ignore
				else:
					continue  # 继续重试选择

			icon_path = Path(f_path)
			if not icon_path.exists():
				messagebox.showerror("错误", "所选文件不存在，请重新选择。")
				continue

			if icon_path.suffix.lower() not in supported_extensions:
				messagebox.showwarning("警告", "请选择一个 .ico 格式的图标文件。")
				continue

			return icon_path  # 成功选择有效图标


class ConfigMgr(object) :
	def __init__(self, configs: dict) :
		self.configs: dict = configs

	def init_default(self) :
		self.configs.setdefault('portable_mode', 'env')
		self.configs.setdefault('data_dir', '{runtime}/pdata')
		self.configs.setdefault('mapPath', [])
		self.configs.setdefault('ext_env', {})
		self.configs.setdefault('executable_path', '{runtime}/Mindustry/Mindustry.exe')
		self.configs.setdefault('RunAsAdministrator', 'no')


class PappBuilder(object) :
	def __init__(self, app_name: Path, build_dir: Path) :
		self.app_name: Path = app_name
		self.build_dir: Path = build_dir
		self.build_dir.mkdir(exist_ok=True)


def main(app_name=None, *, icon=None) :
	cmake_build = CmakeBuilder()
	cmake_build.clean()
	
	if app_name is None :
		app_name = input('Enter your PortableApp name: ')

	if icon is None :
		if (input('Choose a icon? [Y/n] ')+'y')[0] not in 'Nn' :
			root = tk.Tk()
			root.withdraw()
			root.call('wm', 'attributes', '.', '-topmost', True)  # 置于顶层

			try:
				icon_path = IconMgr(Path(__file__).parent.parent / 'icon.ico').tk_select_icon(root)
				if icon_path:
					print(f"Selected icon: {icon_path}")
					# TODO: 这里可以将图标应用到你的可执行文件（例如使用 PyInstaller 或 resource 工具）
				else:
					print("Using default icon.")
			finally:
				root.destroy()

		else :
			print('Using default icon.')


if __name__ == '__main__' :
	main()