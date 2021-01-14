# config.py

def can_build(env, platform):
    return platform=="windows"

def configure(env):
    env.Append(CPPPATH=["#modules/godouken/googletest/googletest/include"])
    env.Append(CPPPATH=["#modules/godouken/googletest/googlemock/include"])
    if env["platform"] == "windows":
        googlelib_test = "gtest"
        googlelib_mock = "gmock"
        googlelib_path = "#modules/godouken/googletest/bin/lib/Release"
        if env["CC"] == "cl":
            googlelib_test += ".lib"
            googlelib_mock += ".lib"
            env.Append(LINKFLAGS=[ googlelib_test, googlelib_mock ])
        else:
            env.Append(LIBS=[ googlelib_test, googlelib_mock ])
        
        env.Append(LIBPATH=[ googlelib_path ])
