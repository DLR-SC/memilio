from clang.cindex import Config
import subprocess

def try_set_libclang_path(path):
    """
    Try to set the file_path for the libclang library. 
    If its already set, the returned Exception gets catched and discarded.
    """
    # Check if path was set in config. If not, try to get it with cmd.
    if (not path):
        clang_cmd = ["clang", '-print-file-name=libclang.so']
        clang_cmd_result = subprocess.check_output(clang_cmd)
        path = clang_cmd_result.rstrip()
    try:
        Config.set_library_file(path)
    except Exception as e:
        if str(e) != "library file must be set before before using any other functionalities in libclang." :
            raise(e)

def get_base_class(base_type):
    result = [base_type]
    for i in range(base_type.get_num_template_arguments()):
        result.append(get_base_class(base_type.get_template_argument_type(i)))
    return result

def get_base_class_string(base_type):
    #result = [base_type.spelling.split("<")[0]]
    result = [base_type.spelling]
    for i in range(base_type.get_num_template_arguments()):
        result.append(get_base_class_string(base_type.get_template_argument_type(i)))
    return result

def indent(level):
    """ 
    Indentation string for pretty-printing
    """ 
    return '  '*level

def output_cursor(cursor, level):
    """ 
    Low level cursor output
    """
    spelling = ''
    displayname = ''

    if cursor.spelling:
        spelling = cursor.spelling
    if cursor.displayname:
        displayname = cursor.displayname
    kind = cursor.kind

    print(indent(level) + spelling, '<' + str(kind) + '>')
    print(indent(level+1) + '"'  + displayname + '"')


def output_cursor_and_children(cursor, level=0):
    """ 
    Output this cursor and its children with minimal formatting.
    """
    output_cursor(cursor, level)
    if cursor.kind.is_reference():
        print(indent(level) + 'reference to:')
        output_cursor(cursor.referenced, level+1)

    # Recurse for children of this cursor
    has_children = False
    for c in cursor.get_children():
        if not has_children:
            print(indent(level) + '{')
            has_children = True
        output_cursor_and_children(c, level+1)

    if has_children:
        print(indent(level) + '}')

def indent(level):
    """ 
    Indentation string for pretty-printing
    """ 
    return '  '*level

def output_cursor_file(cursor, f, level):
    """ 
    Low level cursor output
    """
    spelling = ''
    displayname = ''

    if cursor.spelling:
        spelling = cursor.spelling
    if cursor.displayname:
        displayname = cursor.displayname
    kind = cursor.kind

    f.write(indent(level) + spelling + ' <' + str(kind) + '> ')
    if cursor.location.file:
        f.write(cursor.location.file.name + '\n')
    f.write(indent(level+1) + '"'  + displayname + '"\n')

def output_cursor_and_children_file(cursor, f, level=0):
    """ 
    Output this cursor and its children with minimal formatting.
    """
    output_cursor_file(cursor, f, level)
    if cursor.kind.is_reference():
        f.write(indent(level) + 'reference to:\n')
        output_cursor_file(cursor.referenced, f, level+1)

    # Recurse for children of this cursor
    has_children = False
    for c in cursor.get_children():
        if not has_children:
            f.write(indent(level) + '{\n')
            has_children = True
        output_cursor_and_children_file(c, f, level+1)

    if has_children:
        f.write(indent(level) + '}\n')
