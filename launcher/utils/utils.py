def trans_to_bool(val):
    if type(val) == bool:
        return val
    else:
        return str(val).lower() == 'true' and True or False
