from persistence import *

def main():
    print("Activities")
    list=repo.execute_command("SELECT * FROM activities ORDER BY date ASC;")
    for i in list:
        a=i[0]
        b=i[1]
        c=i[2]
        d=i[3].decode()
        print("({}, {}, {}, '{}')".format(a,b,c,d))

    print("Branches")
    list=repo.execute_command("SELECT * FROM branches ORDER BY id ASC;")
    for i in list:
        a=i[0]
        b=i[1].decode()
        c=i[2]
        print("({}, '{}', {})".format(a,b,c))

    print("Employees")
    list=repo.execute_command("SELECT * FROM employees ORDER BY id ASC;")
    for i in list:
        a=i[0]
        b=i[1].decode()
        c=i[2]
        d=i[3]
        print("({}, '{}', {}, {})".format(a,b,c,d))        


    print("Products")
    list=repo.execute_command("SELECT * FROM products ORDER BY id ASC;")
    for i in list:
        a=i[0]
        b=i[1].decode()
        c=i[2]
        d=i[3]
        print("({}, '{}', {}, {})".format(a,b,c,d))

    print("Suppliers")
    list=repo.execute_command("SELECT * FROM suppliers ORDER BY id ASC;")
    for i in list:
        a=i[0]
        b=i[1].decode()
        c=i[2].decode()
        print("({}, '{}', '{}')".format(a,b,c))    


    list=repo.execute_command("SELECT e.name, e.salary, b.location, COALESCE((SELECT SUM(ABS(p.price) * ABS(a.quantity)) FROM activities a JOIN products p ON a.product_id = p.id WHERE a.activator_id = e.id), 0) AS total_sales FROM employees e JOIN branches b ON e.branche = b.id ORDER BY e.name ASC;")
    print("\nEmployees report")
    for i in list:
        a=i[0].decode()
        b=i[1]
        c=i[2].decode()
        d=i[3]
        print("{} {} {} {}".format(a,b,c,d))

    list=repo.execute_command("SELECT a.date, p.description, a.quantity, CASE WHEN a.activator_id IN (SELECT id FROM employees) THEN (SELECT name FROM employees WHERE id = a.activator_id) ELSE 'None' END as seller, CASE WHEN a.activator_id IN (SELECT id FROM suppliers) THEN (SELECT name FROM suppliers WHERE id = a.activator_id) ELSE 'None' END as supplier FROM activities a JOIN products p ON a.product_id = p.id ORDER BY a.date ASC;")
    print("\nActivities report")
    for i in list:
        a=i[0].decode()
        b=i[1].decode()
        c=i[2]
        d=i[3].decode()
        e=i[4].decode()
        if(d=="None"):
            print("('{}', '{}', {}, {}, '{}')".format(a,b,c,d,e))
        else:
            print("('{}', '{}', {}, '{}', {})".format(a,b,c,d,e))


if __name__ == '__main__':
    main()