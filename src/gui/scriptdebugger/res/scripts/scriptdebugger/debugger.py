""" python debugger script
    this file contain core class of debugger and all related classes
"""

import fnmatch
import sys
import io
import os
from inspect import CO_GENERATOR
import threading
import time
from enum import Enum
import timeit
from PythonQt import *

__all__ = ["Command", "PyDbGExc", "PyDbG", "BreakPoint", "Debugger", "Debugger_Thread", "Profiler"]


class Profiler:
    def __init__(self):
        pass

    def executeTime(self, code='pass', precode='pass', number=1):
        exet_time = timeit.timeit(setup=precode, stmt=code, number=number)
        print("Execute time is : ", exet_time)
        return exet_time

class Command:
    class CommandType(Enum):
        NoCommand = 0
        Stop = 1
        Continue = 2
        StepOver = 3
        StepInto = 4
        StepOut = 5

    currCmd = CommandType.NoCommand
    lock = threading.Lock()

    def __init__(self):
        pass


    def _lock(self):
        Command.lock.acquire()


    def _unlock(self):
        Command.lock.release()

    def reset(self):
        self._lock()
        Command.currCmd = Command.CommandType.NoCommand
        self._unlock()

    def set(self, _cmd):
        self._lock()
        Command.currCmd = _cmd
        self._unlock()

    def get(self):
        return Command.currCmd


class DebugInfo:
    variables = {}
    currentline_no = 0

    def __init__(self):
        pass

cmd = Command()


class PyDbGExc(Exception):
    """Exception to give up completely."""


class PyDbG:
    def __init__(self, skip=None):
        self.skip = set(skip) if skip else None
        self.breaks = {}   # set of break points {(filename,[line numbers])}
        self.fncache = {}  # file name cache : - > indicate all script file names
        self.frame_returning = None  # determine the returning frame

    def canonic(self, filename):
        # this function convert filename to real path
        if filename == "<" + filename[1:-1] + ">":
            return filename
        canonic = self.fncache.get(filename)  # if file is not exist so it returns the existence file
        if not canonic:
            canonic = os.path.abspath(filename)
            canonic = os.path.normcase(canonic)
            # before returning we add it to (fncashe) for next usage and improving performance
            self.fncache[filename] = canonic
        return canonic

    def reset(self):
        # this function reset internal structures for further usage
        import linecache
        linecache.checkcache()
        self.botframe = None  # notice that after creating class we should use this method
        self._set_stopinfo(None, None)

    def _set_stopinfo(self, stopframe, returnframe, stoplineno=0):
        # internal function for keeping some information about STOP: 1- setting stop frame 2- return frame
        self.stopframe = stopframe
        self.returnframe = returnframe
        self.quitting = False
        self.stoplineno = stoplineno  # -1 : don't stop, stoplineno >= 0>= 0 : stop at line

    def trace_dispatch(self, frame, event, arg):
        # this function has been used for tracing python scripts so never edit it or even touch it.
        # base on event we travels in some where
        if self.quitting:
            return
        if event == 'line':
            return self.dispatch_line(frame)
        if event == 'call':
            return self.dispatch_call(frame, arg)
        if event == 'return':
            return self.dispatch_return(frame, arg)
        if event == 'exception':
            return self.dispatch_exception(frame, arg)
        if event == 'c_call':
            return self.trace_dispatch
        if event == 'c_exception':
            return self.trace_dispatch
        if event == 'c_return':
            return self.trace_dispatch
        print('bdb.Bdb.dispatch: unknown debugging event:', repr(event))
        return self.trace_dispatch

    def dispatch_line(self, frame):
        if self.stop_here(frame) or self.break_here(frame):
            self.user_line(frame)
            # after returning we check if user terminated the tracing or not
            if self.quitting: raise PyDbGExc
        # If there is neither stop condition and break line so we return to
        # the main trace function for further tracing
        return self.trace_dispatch

    def dispatch_call(self, frame, arg):
        if self.botframe is None:
            self.botframe = frame.f_back  # keeping of return frame for more tracing
            return self.trace_dispatch
        if not (self.stop_here(frame) or self.break_anywhere(frame)):
            # since there are not any breaks in this function, No need to trace it.
            return
        # Ignore call events in generator except when stepping.
        if self.stopframe and frame.f_code.co_flags & CO_GENERATOR:
            return self.trace_dispatch
        self.user_call(frame, arg)
        if self.quitting: raise PyDbGExc
        return self.trace_dispatch

    def dispatch_return(self, frame, arg):
        # before returning from function
        if self.stop_here(frame) or frame == self.returnframe:
            if self.stopframe and frame.f_code.co_flags & CO_GENERATOR:
                return self.trace_dispatch
            try:
                # before return from this frame we save this frame for more processing on it
                self.frame_returning = frame
                self.user_return(frame, arg)
            finally:
                self.frame_returning = None
            if self.quitting: raise PyDbGExc
            # The user issued a 'next' or 'until' command.
            if self.stopframe is frame and self.stoplineno != -1:
                self._set_stopinfo(None, None)
        return self.trace_dispatch

    def dispatch_exception(self, frame, arg):
        if self.stop_here(frame):
            # When stepping with next/until/return in a generator frame, skip
            # the internal StopIteration exception (with no traceback)
            # triggered by a subiterator run with the 'yield from' statement.
            if not (frame.f_code.co_flags & CO_GENERATOR
                    and arg[0] is StopIteration and arg[2] is None):
                self.user_exception(frame, arg)
                if self.quitting: raise PyDbGExc
        # Stop at the StopIteration or GeneratorExit exception when the user
        # has set stopframe in a generator by issuing a return command, or a
        # next/until command at the last statement in the generator before the
        # exception.
        elif (self.stopframe and frame is not self.stopframe
                and self.stopframe.f_code.co_flags & CO_GENERATOR
                and arg[0] in (StopIteration, GeneratorExit)):
            self.user_exception(frame, arg)
            if self.quitting: raise PyDbGExc

        return self.trace_dispatch

    def user_call(self, frame, argument_list):
        pass

    def user_line(self, frame):
        pass

    def user_return(self, frame, return_value):
        pass

    def user_exception(self, frame, exc_info):
        pass

    def stop_here(self, frame):
        if self.skip and \
               self.is_skipped_module(frame.f_globals.get('__name__')):
            return False
        if frame is self.stopframe:
            if self.stoplineno == -1:
                return False
            return frame.f_lineno >= self.stoplineno
        if not self.stopframe:
            return True
        return False

    def is_skipped_module(self, module_name):
        for pattern in self.skip:
            if fnmatch.fnmatch(module_name, pattern):
                return True
        return False

    def break_here(self, frame):
        # if there is any break point we will set self.currentbp to point it
        filename = self.canonic(frame.f_code.co_filename)
        if filename not in self.breaks:
            return False
        lineno = frame.f_lineno
        if lineno not in self.breaks[filename]:
            # The line itself has no breakpoint, but maybe the line is the
            # first line of a function with breakpoint set by function name.
            lineno = frame.f_code.co_firstlineno
            if lineno not in self.breaks[filename]:
                return False

        # flag says ok to delete temp. bp
        (bp, flag) = effective(filename, lineno, frame)
        if bp:
            self.currentbp = bp.number
            if (flag and bp.temporary):
                self.clear(str(bp.number))
            return True
        else:
            return False

    def clear(self, arg):
        raise NotImplementedError("subclass must implement clear()")

    def break_anywhere(self, frame):
        return self.canonic(frame.f_code.co_filename) in self.breaks

    # Derived classes can call the following methods
    # to affect the stepping state.
    def set_until(self, frame, lineno=None):
        if lineno is None:
            lineno = frame.f_lineno + 1
        self._set_stopinfo(frame, frame, lineno)

    def set_step(self):
        if self.frame_returning:
            caller_frame = self.frame_returning.f_back
            if caller_frame and not caller_frame.f_trace:
                caller_frame.f_trace = self.trace_dispatch
        self._set_stopinfo(None, None)

    def set_next(self, frame):
        # Stop on the next line
        self._set_stopinfo(frame, None)

    def set_return(self, frame):
        # Stop when returning from the given frame.
        if frame.f_code.co_flags & CO_GENERATOR:
            self._set_stopinfo(frame, None, -1)
        else:
            self._set_stopinfo(frame.f_back, frame)

    def set_trace(self, frame=None):
        if frame is None:
            frame = sys._getframe().f_back
        self.reset()
        while frame:
            frame.f_trace = self.trace_dispatch
            self.botframe = frame
            frame = frame.f_back
        self.set_step()
        sys.settrace(self.trace_dispatch)

    def set_continue(self):
        # Don't stop except at breakpoints or when finished
        self._set_stopinfo(self.botframe, None, -1)
        if not self.breaks:
            # no breakpoints; run without debugger overhead
            sys.settrace(None)
            frame = sys._getframe().f_back
            while frame and frame is not self.botframe:
                del frame.f_trace
                frame = frame.f_back

    def set_quit(self):
        self.stopframe = self.botframe
        self.returnframe = None
        self.quitting = True
        sys.settrace(None)

    def set_break(self, filename, lineno, temporary=False, cond=None, funcname=None):
        filename = self.canonic(filename)
        import linecache # Import as late as possible
        line = linecache.getline(filename, lineno)
        if not line:
            return 'Line %s:%d does not exist' % (filename, lineno)
        list = self.breaks.setdefault(filename, [])
        if lineno not in list:
            list.append(lineno)
        bp = BreakPoint(filename, lineno, temporary, cond, funcname)

    def _prune_breaks(self, filename, lineno):
        if (filename, lineno) not in BreakPoint.bplist:
            self.breaks[filename].remove(lineno)
        if not self.breaks[filename]:
            del self.breaks[filename]

    def clear_break(self, filename, lineno):
        filename = self.canonic(filename)
        if filename not in self.breaks:
            return 'There are no breakpoints in %s' % filename
        if lineno not in self.breaks[filename]:
            return 'There is no breakpoint at %s:%d' % (filename, lineno)
        # If there's only one bp in the list for that file,line
        # pair, then remove the breaks entry
        for bp in BreakPoint.bplist[filename, lineno][:]:
            bp.deleteMe()
        self._prune_breaks(filename, lineno)

    def clear_bpbynumber(self, arg):
        try:
            bp = self._get_bpbynumber(arg)
        except ValueError as err:
            return str(err)
        bp.deleteMe()
        self._prune_breaks(bp.file, bp.line)

    def clear_all_file_breaks(self, filename):
        filename = self.canonic(filename)
        if filename not in self.breaks:
            return 'There are no breakpoints in %s' % filename
        for line in self.breaks[filename]:
            blist = BreakPoint.bplist[filename, line]
            for bp in blist:
                bp.deleteMe()
        del self.breaks[filename]

    def clear_all_breaks(self):
        if not self.breaks:
            return 'There are no breakpoints'
        for bp in BreakPoint.bpbynumber:
            if bp:
                bp.deleteMe()
        self.breaks = {}

    def _get_bpbynumber(self, arg):
        if not arg:
            raise ValueError('Breakpoint number expected')
        try:
            number = int(arg)
        except ValueError:
            raise ValueError('Non-numeric breakpoint number %s' % arg)
        try:
            bp = BreakPoint.bpbynumber[number]
        except IndexError:
            raise ValueError('Breakpoint number %d out of range' % number)
        if bp is None:
            raise ValueError('Breakpoint %d already deleted' % number)
        return bp

    def get_break(self, filename, lineno):
        filename = self.canonic(filename)
        return filename in self.breaks and \
            lineno in self.breaks[filename]

    def get_breaks(self, filename, lineno):
        filename = self.canonic(filename)
        return filename in self.breaks and \
            lineno in self.breaks[filename] and \
            BreakPoint.bplist[filename, lineno] or []

    def get_file_breaks(self, filename):
        filename = self.canonic(filename)
        if filename in self.breaks:
            return self.breaks[filename]
        else:
            return []

    def get_all_breaks(self):
        return self.breaks

    def run(self, cmd, path="<string>", globals=None, locals=None):
        if globals is None:
            import __main__
            globals = __main__.__dict__
        if locals is None:
            locals = globals
        self.reset()
        if isinstance(cmd, str):
            cmd = compile(cmd, path, "exec")
        sys.settrace(self.trace_dispatch)
        try:
            exec(cmd, globals, locals)
        except PyDbGExc:
            pass
        finally:
            self.quitting = True
            sys.settrace(None)

    def runeval(self, expr, globals=None, locals=None):
        if globals is None:
            import __main__
            globals = __main__.__dict__
        if locals is None:
            locals = globals
        self.reset()
        sys.settrace(self.trace_dispatch)
        try:
            return eval(expr, globals, locals)
        except PyDbGExc:
            pass
        finally:
            self.quitting = True
            sys.settrace(None)

    def runcall(self, func, *args, **kwds):
        self.reset()
        sys.settrace(self.trace_dispatch)
        res = None
        try:
            res = func(*args, **kwds)
        except PyDbGExc:
            pass
        finally:
            self.quitting = True
            sys.settrace(None)
        return res


class BreakPoint:
    next = 1        # Next bp to be assigned
    bplist = {}     # indexed by (file, lineno) tuple
    bpbynumber = [None] # Each entry is None or an instance of Bpt
                # index 0 is unused, except for marking an
                # effective break .... see effective()

    def __init__(self, file, line, temporary=False, cond=None, funcname=None):
        self.funcname = funcname
        # Needed if funcname is not None.
        self.func_first_executable_line = None
        self.file = file    # This better be in canonical form!
        self.line = line
        self.temporary = temporary
        self.cond = cond
        self.enabled = True
        self.ignore = 0
        self.hits = 0
        self.number = BreakPoint.next
        BreakPoint.next += 1
        # Build the two lists
        self.bpbynumber.append(self)
        if (file, line) in self.bplist:
            self.bplist[file, line].append(self)
        else:
            self.bplist[file, line] = [self]

    def deleteMe(self):
        index = (self.file, self.line)
        self.bpbynumber[self.number] = None   # No longer in list
        self.bplist[index].remove(self)
        if not self.bplist[index]:
            # No more bp for this file:line, so we delete this entry.
            del self.bplist[index]

    def enable(self):
        self.enabled = True

    def disable(self):
        self.enabled = False

    def bpprint(self, out=None):
        if out is None:
            out = sys.stdout
        print(self.bpformat(), file=out)

    def bpformat(self):
        if self.temporary:
            disp = 'del  '
        else:
            disp = 'keep '
        if self.enabled:
            disp = disp + 'yes  '
        else:
            disp = disp + 'no   '
        ret = '%-4dbreakpoint   %s at %s:%d' % (self.number, disp,
                                                self.file, self.line)
        if self.cond:
            ret += '\n\tstop only if %s' % (self.cond,)
        if self.ignore:
            ret += '\n\tignore next %d hits' % (self.ignore,)
        if self.hits:
            if self.hits > 1:
                ss = 's'
            else:
                ss = ''
            ret += '\n\tbreakpoint already hit %d time%s' % (self.hits, ss)
        return ret

    def __str__(self):
        return 'breakpoint %s at file %s and line %s' % (self.number, self.file, self.line)


def checkfuncname(b, frame):
    if not b.funcname:
        # Breakpoint was set via line number.
        if b.line != frame.f_lineno:
            # Breakpoint was set at a line with a def statement and the function
            # defined is called: don't break.
            return False
        return True

    # Breakpoint set via function name.

    if frame.f_code.co_name != b.funcname:
        # It's not a function call, but rather execution of def statement.
        return False

    # We are in the right frame.
    if not b.func_first_executable_line:
        # The function is entered for the 1st time.
        b.func_first_executable_line = frame.f_lineno

    if  b.func_first_executable_line != frame.f_lineno:
        # But we are not at the first line number: don't break.
        return False
    return True


def effective(file, line, frame):
    possibles = BreakPoint.bplist[file, line]
    for b in possibles:
        if not b.enabled:
            continue
        if not checkfuncname(b, frame):
            continue
        # Count every hit when bp is enabled
        b.hits += 1
        if not b.cond:
            # If unconditional, and ignoring go on to next, else break
            if b.ignore > 0:
                b.ignore -= 1
                continue
            else:
                # breakpoint and marker that it's ok to delete if temporary
                return (b, True)
        else:
            # Conditional bp.
            # Ignore count applies only to those bpt hits where the
            # condition evaluates to true.
            try:
                val = eval(b.cond, frame.f_globals, frame.f_locals)
                if val:
                    if b.ignore > 0:
                        b.ignore -= 1
                        # continue
                    else:
                        return (b, True)
                # else:
                #   continue
            except:
                # if eval fails, most conservative thing is to stop on
                # breakpoint regardless of ignore count.  Don't delete
                # temporary, as another hint to user.
                return (b, False)
    return (None, None)


def set_trace():
    PyDbG().set_trace()


class Debugger(PyDbG):

    myrun = 0
    info = DebugInfo()

    def print_dictionary(dct):
        for item, value in list(dct.items()):
            print("{} : {}".format(item, value))

    def user_call(self, frame, args):
        name = frame.f_code.co_name or "<unknown>"
        filename = self.canonic(frame.f_code.co_filename)
        Debugger.info.currentline_no = frame.f_lineno
        print("call", name, args)
        while True:
            if cmd.get() == Command.CommandType.StepOver:
                cmd.reset()
                self.set_next(frame)
                break
            if cmd.get() == Command.CommandType.StepInto:
                cmd.reset()
                #self.set_step()
                self.set_trace(frame)
                break
            elif cmd.get() == Command.CommandType.Continue:
                cmd.reset()
                self.set_continue()
                break
            elif cmd.get() == Command.CommandType.Stop:
                self.set_quit()
                cmd.reset()
                break
            else:
                time.sleep(0.01)

    def user_line(self, frame):
        if Debugger.myrun:
            Debugger.myrun = 0
            self.set_trace()
        else:
            name = frame.f_code.co_name or "<unknown>"
            #print("name issssss",name)
            filename = self.canonic(frame.f_code.co_filename)
            Debugger.info.currentline_no = frame.f_lineno
            print("break at", filename, frame.f_lineno, "in", name)
            if name != '<module>' and name != 'run':
                Debugger.info.variables = frame.f_locals
                # if 'c' in dict(frame.f_locals).keys():
                #     print("yes it is")
                #     dict(frame.f_locals)['c'] = 14
            else:
                Debugger.info.variables = {}
            #print(str(frame.f_lineno) + " -> " + str(frame.f_locals))
            # if name != '<module>' and frame != frame.f_globals:
            #     print("Local Parameters :: \n")
            #     for item, value in list(frame.f_locals.items()):
            #         print("{} : {}".format(item, value))

            print("continue ...")
            while True:
                if cmd.get() == Command.CommandType.StepOver:
                    cmd.reset()
                    self.set_next(frame)
                    break
                if cmd.get() == Command.CommandType.StepInto:
                    cmd.reset()
                    #self.set_step()
                    self.set_trace(frame)
                    break
                elif cmd.get() == Command.CommandType.Continue:
                    cmd.reset()
                    self.set_continue()
                    break
                elif cmd.get() == Command.CommandType.Stop:
                    self.set_quit()
                    cmd.reset()
                    break
                else:
                    time.sleep(0.01)

    def user_return(self, frame, return_value):
        name = frame.f_code.co_name or "<unknown>"
        Debugger.info.currentline_no = frame.f_lineno
        print("return from ", name, return_value)
        print("continue ...")
        #self.set_continue()

    def user_exception(self, frame, exc_info):
        name = frame.f_code.co_name or "<unknown>"
        Debugger.info.currentline_no = frame.f_lineno
        print("exception in ", name, exc_info)
        print("continue ...")
        #self.set_continue()


class Debugger_Thread(threading.Thread):
    dbg = Debugger()
    filepath = "script.py"

    def __init__(self):
        threading.Thread.__init__(self)
        self.dbg.myrun = 0
        self.dbg.reset()

    def setScript(self, _script):
        self.dbg.reset()
        self.script = _script
        fp = io.open(Debugger_Thread.filepath, 'w', encoding='utf8')
        fp.write(self.script)
        fp.close()

    def _start(self):
        self.dbg.myrun = 1

    def _stop(self):
        self.dbg.myrun = 0
        self.dbg.reset()
        self.dbg.set_quit()

    def run(self):
        self._start()
        self.dbg.run(self.script, Debugger_Thread.filepath)
        self._stop()
        print("Stop Debug Thread")


def init(script):
    cmd.reset()
    if not os.path.exists(os.path.join(os.getcwd(), "debugfiles")):
        os.mkdir(os.path.join(os.getcwd(), "debugfiles"))
    filepath = os.path.join(os.getcwd(), "debugfiles/script.py")
    fp = io.open(filepath, "w", encoding='utf8')
    fp.write(script)
    fp.close()


def debugger(script):
    cmd.reset()
    Debugger_Thread.dbg.info.currentline_no = 0
    Debugger_Thread.dbg.info.variables = {}
    sample_thread = Debugger_Thread()
    sample_thread.setScript(script)
    sample_thread.start()
    Debugger_Thread.dbg.info.currentline_no = 0
    Debugger_Thread.dbg.info.variables = {}
    del(sample_thread)


# this function use for step in to function.
def dbg_stepOver():
    global cmd
    cmd.set(Command.CommandType.StepOver)
    time.sleep(0.05)


def dbg_stepInto():
    global cmd
    cmd.set(Command.CommandType.StepInto)
    time.sleep(0.05)


def dbg_stepOut():
    global cmd
    cmd.set(Command.CommandType.StepOut)
    time.sleep(0.05)


def dbg_stop():
    global cmd
    cmd.set(Command.CommandType.Stop)
    time.sleep(0.05)


def dbg_continue():
    global cmd
    cmd.set(Command.CommandType.Continue)
    time.sleep(0.05)


def dbg_set_break(lineno):
    print(Debugger_Thread.dbg.set_break(Debugger_Thread.filepath, lineno))


def dbg_clear_break(lineno):
    print(Debugger_Thread.dbg.clear_break(Debugger_Thread.filepath, lineno))


def dbg_clear_all_breaks():
    Debugger_Thread.dbg.clear_all_breaks()


def dbg_currentline():
    return Debugger_Thread.dbg.info.currentline_no


def dbg_debugInfo():
    if bool(Debugger_Thread.dbg.info.variables):
        return str(Debugger_Thread.dbg.info.variables)
    else:
        return ""


def dbg_execTime(code='pass', precode='pass', number=1):
    profiler = Profiler()
    return profiler.executeTime(code, precode, number)



