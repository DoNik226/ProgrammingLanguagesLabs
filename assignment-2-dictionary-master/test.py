import subprocess
import os

inputs = ["", "first", "second", "third", "a", "a" * 257]
expected_outputs = ["", "I", "hate", "web", "", ""]
expected_errors = ["Element not found", "", "", "", "Element not found", "Invalid input"]

errors = []

if os.path.exists("./main"):
    print("Starting test")

    for i in range(len(inputs)):
        process = subprocess.Popen(["./main"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = process.communicate(input=inputs[i].encode())

        stdout = stdout.decode().strip()
        stderr = stderr.decode().strip()

        if (stdout == expected_outputs[i] and stderr == expected_errors[i]):
            print("_", end="")
        else:
            print("F", end="")
            if stdout != expected_outputs[i]:
                errors.append("Wrong stdout " + stdout + ", expected " + expected_outputs[i])
            if stderr != expected_errors[i]:
                errors.append("Wrong stderr " + stderr + " expected " + expected_errors[i])

    print("\n")
    if len(errors) != 0:
        for string in errors:
            print(string)
    else:
        print("All tests passed")
else:
    print("Executable file \"main\" does not exist")
