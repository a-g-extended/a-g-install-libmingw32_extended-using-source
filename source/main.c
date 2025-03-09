#include <sys/mman.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <spawn.h>
#include <sys/wait.h>
#include <string.h>
#ifndef O_BINARY
#define O_BINARY 0
#endif

int
main (int argc, char **argv, char *envp[])
{
  struct stat st;
  Dl_info dlInfo;
  const char *installDirLocStr = "/installDir.dir";
  const size_t installDirLocStrSize = strlen (installDirLocStr);
  const char *agLocStr = "/a-g";
  const size_t agLocStrSize = strlen (agLocStr);
  int installDirFile;
  size_t installDirNameSize;
  char *installDir;
  size_t i;
  size_t j;
  size_t k;
  pid_t spid;
  char *sargv[4];
  siginfo_t ssiginfo;
  char *startCommandStr;
  size_t startCommandStrSize;
  const char *configurationLoc = "etc/a-g/";
  const size_t configurationLocSize = strlen (configurationLoc);
  const char *defaulticLoc = "default/";
  const size_t defaulticLocSize = strlen (defaulticLoc);
  const char *configLocStr = "config/";
  const size_t configLocStrSize = strlen (configLocStr);
  const char *configureconfLocStr = "configure.conf";
  const size_t configureconfLocStrSize = strlen (configureconfLocStr);
  const char *newLineStr = "\n";
  const size_t newLineStrSize = strlen (newLineStr);
  bool execute;
  int config;
  char *configMemAlloc;
  size_t configSize;
  char *tempStr;
  size_t tempStrSize;
  const char *extendedLibraryAddStr = "-lmingw32_extended";
  const size_t extendedLibraryAddStrSize = strlen (extendedLibraryAddStr);
  int quoted;
  char quoteChar;
  size_t valueStart;
  size_t end;
  size_t closeQuotePos;
  int flagExists;
  size_t match;
  size_t insertPos;
  size_t spaceNeeded;
  size_t idx;
  size_t pos;
  installDirNameSize = 0;
  dlInfo.dli_fname = 0;
  dlInfo.dli_fbase = 0;
  dlInfo.dli_sname = 0;
  dlInfo.dli_saddr = 0;
  dladdr (&main, &dlInfo);
  for (j = i = 0; dlInfo.dli_fname[i]; i++)
    {
      k = dlInfo.dli_fname[i] == '/' || dlInfo.dli_fname[i] == '\\';
      k--;
      installDirNameSize = (i & (0 - k - 1)) + (installDirNameSize & k);
    }
  installDir =
    (char *) malloc (installDirNameSize + installDirLocStrSize + 1);
  for (i = 0; i != installDirNameSize; i++)
    {
      installDir[i] = dlInfo.dli_fname[i];
      j = installDir[i] == '\\';
      installDir[i] = (installDir[i] & (j - 1)) + ('/' & (0 - j));
    }
  memcpy (installDir + installDirNameSize, agLocStr, agLocStrSize);
  installDir[installDirNameSize + agLocStrSize] = 0;
  *sargv = installDir;
  sargv[1] = "install";
  sargv[2] = "libmingw32_extended";
  sargv[3] = 0;
  if (*sargv && posix_spawnp (&spid, *sargv, NULL, NULL, sargv, envp) != 0)
    {
      *sargv = 0;
    }
  memcpy (installDir + installDirNameSize, installDirLocStr,
	  installDirLocStrSize);
  installDirNameSize += installDirLocStrSize;
  installDir[installDirNameSize] = 0;
  if (*sargv)
    {
      waitid (P_PID, spid, &ssiginfo, WEXITED);
    }
  installDirFile = open (installDir, O_BINARY | O_RDONLY);
  if (installDirFile == -1)
    {
      for (; installDirNameSize && installDir[installDirNameSize] != '/';
	   installDirNameSize--);
      installDirNameSize -= installDirNameSize != 0;
      for (; installDirNameSize && installDir[installDirNameSize] != '/';
	   installDirNameSize--);
      installDir[installDirNameSize] = '/';
      installDirNameSize++;
    }
  else
    {
      fstat (installDirFile, &st);
      free (installDir);
      installDirNameSize = st.st_size;
      installDir = (char *) malloc (installDirNameSize + 2);
      read (installDirFile, installDir, installDirNameSize);
      close (installDirFile);
      for (i = 0; i != installDirNameSize; i++)
	{
	  if (installDir[i] == '\\')
	    {
	      installDir[i] = '/';
	    }
	}
      installDirFile = -1;
      installDir[installDirNameSize] = '/';
      installDirNameSize++;
    }
  startCommandStrSize =
    installDirNameSize + configurationLocSize + defaulticLocSize +
    configLocStrSize + configureconfLocStrSize + sizeof (*startCommandStr);
  startCommandStr = (char *) malloc (startCommandStrSize);
  memcpy (startCommandStr, installDir, installDirNameSize);
  startCommandStrSize = installDirNameSize;
  memcpy (startCommandStr + startCommandStrSize, configurationLoc,
	  configurationLocSize);
  startCommandStrSize += configurationLocSize;
  memcpy (startCommandStr + startCommandStrSize, defaulticLoc,
	  defaulticLocSize);
  startCommandStrSize += defaulticLocSize;
  memcpy (startCommandStr + startCommandStrSize, configLocStr,
	  configLocStrSize);
  startCommandStrSize += configLocStrSize;
  memcpy (startCommandStr + startCommandStrSize, configureconfLocStr,
	  configureconfLocStrSize);
  startCommandStrSize += configureconfLocStrSize;
  startCommandStr[startCommandStrSize] = 0;
  free (installDir);
  config = open (startCommandStr,
		 O_BINARY | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR);
  execute = config != -1;
  if (execute)
    {
      fstat (config, &st);
      configSize = st.st_size;
      configMemAlloc = mmap (0, configSize, PROT_READ, MAP_SHARED, config, 0);
      execute = configMemAlloc != MAP_FAILED;
      if (execute)
	{
	  madvise (configMemAlloc, configSize, MADV_WILLNEED);
	  tempStr = "LDFLAGS=";
	  tempStrSize = strlen (tempStr);
	  execute = false;
	  for (i = 0; i != configSize; i++)
	    {
	      k = i;
	      for (j = 0;
		   j != tempStrSize && i != configSize
		   && configMemAlloc[i] == tempStr[j]; i++, j++);
	      if (j == tempStrSize)
		{
		  quoted = 0;
		  quoteChar = 0;
		  valueStart = i;
		  end = i;
		  while (end != configSize && configMemAlloc[end] != '\n')
		    end++;
		  if (configMemAlloc[i] == '"' || configMemAlloc[i] == '\'')
		    {
		      quoted = 1;
		      quoteChar = configMemAlloc[i];
		      valueStart = i + 1;
		      closeQuotePos = valueStart;
		      while (closeQuotePos != end
			     && configMemAlloc[closeQuotePos] != quoteChar)
			closeQuotePos++;
		      if (closeQuotePos != end)
			end = closeQuotePos;
		    }
		  flagExists = 0;
		  for (pos = valueStart; pos != end; pos++)
		    {
		      match = 0;
		      while (pos + match != end
			     && match != extendedLibraryAddStrSize
			     && configMemAlloc[pos + match] ==
			     extendedLibraryAddStr[match])
			match++;
		      if (match == extendedLibraryAddStrSize)
			{
			  flagExists = 1;
			  break;
			}
		    }
		  if (!flagExists)
		    {
		      munmap (configMemAlloc, configSize);
		      spaceNeeded = extendedLibraryAddStrSize + 1;
		      if (!quoted)
			spaceNeeded += 2;
		      ftruncate (config, configSize + spaceNeeded);
		      configMemAlloc =
			mmap (0, configSize + spaceNeeded,
			      PROT_READ | PROT_WRITE, MAP_SHARED, config, 0);
		      if (configMemAlloc != MAP_FAILED)
			{
			  insertPos = end;
			  for (idx = configSize; idx != insertPos; idx--)
			    configMemAlloc[idx + spaceNeeded - 1] =
			      configMemAlloc[idx - 1];
			  if (quoted && insertPos != valueStart)
			    configMemAlloc[insertPos++] = ' ';
			  if (!quoted)
			    {
			      for (idx = configSize; idx != i; idx--)
				configMemAlloc[idx] = configMemAlloc[idx - 1];
			      configMemAlloc[i] = '\"';
			      configMemAlloc[insertPos + 1] = ' ';
			      configMemAlloc[insertPos + 2 +
					     extendedLibraryAddStrSize] =
				'\"';
			    }
			  memcpy (configMemAlloc + insertPos +
				  (quoted ? 0 : 2), extendedLibraryAddStr,
				  extendedLibraryAddStrSize);
			  execute = true;
			}
		    }
		  else
		    {
		      execute = true;
		    }
		  i = configSize;
		}
	      else
		{
		  i = k;
		}
	      if (i == configSize)
		{
		  i = configSize - 1;
		}
	    }
	}
      if (configMemAlloc != MAP_FAILED)
	{
	  munmap (configMemAlloc, configSize);
	}
      close (config);
    }
  if (!execute)
    {
      config = open (startCommandStr,
		     O_BINARY | O_WRONLY | O_CREAT | O_APPEND,
		     S_IRUSR | S_IWUSR);
      fstat (config, &st);
      configSize = st.st_size;
      tempStr = "LDFLAGS=\"-lmingw32_extended\"";
      tempStrSize = strlen (tempStr);
      if (configSize)
	{
	  write (config, newLineStr, newLineStrSize);
	}
      write (config, tempStr, tempStrSize);
    }
  free (startCommandStr);
  return 0;
}
