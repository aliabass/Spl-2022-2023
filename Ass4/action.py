from persistence import *

import sys

def main(args : list[str]):
    inputfilename : str = args[1]
    with open(inputfilename) as inputfile:
        for line in inputfile:
            splittedline : list[str] = line.strip().split(", ")
            a=Activitie(splittedline[0],splittedline[1],splittedline[2],splittedline[3])
            if(int(splittedline[1])>0):
                repo.activitie.insert(a)
                query="UPDATE products SET quantity=quantity+ {} WHERE id={}".format(a.quantity,a.product_id)
                repo.execute_command(query)
            else:
                x=repo.product.find(id=a.product_id)
                row=x[0]
                if(int(row.quantity)+int(a.quantity)>=0):
                    repo.activitie.insert(a)
                    query="UPDATE products SET quantity=quantity + {} WHERE id={}".format(a.quantity,a.product_id)
                    repo.execute_command(query)


                    

if __name__ == '__main__':
    main(sys.argv)