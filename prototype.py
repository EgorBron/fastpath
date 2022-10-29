import os # winapi sysenv
# import ctypes
import sys # cmd argv
import ujson # json
import re # regexpr
BACKSLASH = '\\'
def raiser(exc: Exception):
    """Raises an exception"""
    raise exc
def cmdmsg(typed: str, message: str) -> str:
    finalized = f'FASTPATH - {typed}:\n\t{message}'
    print(finalized)
    return finalized
class FastpathDir:
    """Represents a Fastpath directory

    Parameters:
    [str]
    """
    def __init__(self, path:str, /) -> None:
        self.__dpath = path
        cfgfile = open(path+'\\CONFIG.fph')
        self.__cfg:dict = ujson.loads(cfgfile.read().replace('\n', "") or raiser(IOError(f"directory {path} don\'t contain configuration")))
        cfgfile.close()
        self.__dpath = FastpathDir.regexp_replaces_variables(self.__dpath)
    @staticmethod
    def regexp_replaces_variables(inp: str) -> str:
        ret = inp
        for matched in re.findall(r'%[\.\w\d-]+%', inp):
            ret = ret.replace(matched, os.environ.get(matched.replace(r'%', '')))
        return ret
    def files(self) -> list:
        pafiles = []
        if self.__cfg.get('SETTINGS').get('INCLUDE_LOCAL'):
            for lofile in os.listdir(self.__dpath):
                if not lofile.endswith('.fph'):
                    pafiles.append(f'{self.__dpath}{BACKSLASH if not self.__dpath.endswith(BACKSLASH) else ""}{lofile}')
        for val in self.__cfg.get('DESTINATIONS'):
            dest = self.__cfg.get('DESTINATIONS')[val]
            dest = FastpathDir.regexp_replaces_variables(dest)
            for refile in os.listdir(dest):
                pafiles.append(f'{dest}{BACKSLASH if not dest.endswith(BACKSLASH) else ""}{refile}')
        return pafiles if pafiles else raiser(IOError('directories is empty'))
    def destinations(self) -> dict:
        return self.__cfg.get("DESTINATIONS")
def parse_argv(toparse: list) -> dict:
	finite = {}
	last = 'nocategory'
	for i in toparse:
		if i.startswith('-'):
			last = i[1:]
			finite[last] = ""
		else:
			try:
				finite[last] += " "+i
			except: finite[last] = i
	for k in finite:
		finite[k] = finite[k].strip()
		if finite[k] == '': finite[k] = None
	return finite

# main
p = parse_argv(sys.argv[1:])
fpdir = FastpathDir("C:\\fastpath")
nocat = p.get('nocategory')
if p.get('a') or p.get('add'):
    dpath = fpdir._FastpathDir__dpath
    path_to_cfg = f'{dpath}{BACKSLASH if not dpath.endswith(BACKSLASH) else ""}CONFIG.fph'
    old_data = {}
    with open(path_to_cfg, 'r') as file:
        old_data = ujson.load(file)
    with open(path_to_cfg, 'w') as file:
        dataforappend = (p.get('a') or p.get('add')).split()
        old_data["DESTINATIONS"][dataforappend[0]] = " ".join(dataforappend[1:])
        ujson.dump(old_data, file)
else:
    nocat = nocat if nocat else cmdmsg('ERROR', 'No file provided!') and sys.exit(-1)
    for file in fpdir.files():
        if file.endswith(nocat):
            os.system(file)
            break
    else:
        for dest in fpdir.destinations():
            if dest.lower() == nocat:
                os.system(f'explorer {fpdir.destinations()[dest]}')
                break
        else:
            cmdmsg('ERROR', 'Not found!')
# cmdmsg('ERROR', 'Provide workdir first!')
