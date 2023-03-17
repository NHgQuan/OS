#/bin/bash

# calc.sh

#calculate valid operation
cal()
{
    if [[ "$1" == "ANS" ]] ; then
        readFromANSFile
        firstOperant=$ANSFileValue
    else
        firstOperant=$1
    fi

    if [[ "$3" == "ANS" ]] ; then
        readFromANSFile
        secondOperant=$ANSFileValue
    else
        secondOperant=$3
    fi

    if [[ "$2" == "/" ]] ; then
        if [[ "$secondOperant" == "0" ]] ; then
            echo "MATH ERROR"; return ;
        else
            cal_result=`echo "scale=2; $firstOperant $2 $secondOperant" | bc`
        fi
    else
        cal_result=`echo "$firstOperant $2 $secondOperant" | bc`
    fi
    #print result to terminal
    echo $cal_result

    #write to ANS file
    overwriteANSFile $cal_result

    #write to HIST file
    makeNewestHistoryLine $cal_result
    serializeWriteHISTFile
}

#read operation
readOperation()
{
    #printf ">> "
    read -p ">> " num1 opt num2
}

#checkSyntax of operation
checkSyntax()
{
    checkSyntax_re='^[0-9]+$'
    if ! [[ $1 =~ $checkSyntax_re ]] && [[ "$1" != "ANS" ]] ; then
    echo "SYNTAX ERROR"; return 0;
    fi

    if ! [[ $3 =~ $checkSyntax_re ]] && [[ "$3" != "ANS" ]] ; then
    echo "SYNTAX ERROR"; return 0;
    fi

    if [ "$2" != "+" ] && [ "$2" != "-" ] && [ "$2" != "*" ] && [ "$2" != "/" ] && [ "$2" != "%" ] ; then
    echo "SYNTAX ERROR"; return 0;
    fi

    if [ "$4" != "" ] ; then
    echo "SYNTAX ERROR"; return 0;
    fi
    
    return 1;
}

#process unchecked operation
processOperation()
{
    checkSyntax "$num1" "$opt" "$num2"
        if [ $? -eq 1 ] ; then
            cal "$num1" "$opt" "$num2"
        fi
}

#pause program untill press any
pauseUntillPressAny()
{
    read -n 1 -s -r -p ""
}

#create ANS.txt
createANSFile()
{
    if [ ! -f /tmp/ANS.txt ]; then
        touch ANS.txt
        echo "0" > ANS.txt
    fi
    
}

#write to ANS.txt
overwriteANSFile()
{
    #input: variable contain result of calculator
    echo $1 > ANS.txt
}

#read from ANS.txt
readFromANSFile()
{
    read ANSFileValue < ANS.txt
}

#create HIST.txt
createHISTFile()
{
    touch HIST.txt
}

#write to HIST.txt
serializeWriteHISTFile()
{
    #input: variable contain result of calculator
    echo "$newestHistLine" | cat - HIST.txt > temp && mv temp HIST.txt
}

#read from HIST.txt
readFromHISTFile()
{
    read HISTFileValue < HIST.txt
}

#make newest history line
makeNewestHistoryLine()
{
    newestHistLine="$num1"" """$opt""" ""$num2"" = ""$1"
}

#indidate 5 newest history line
indidate5NewestHistoryLine()
{
    index=0;

    while read newestHistLine ; do
        echo "$newestHistLine"
        index=`echo ""$index"+1" | bc`
        if [[ "$index" ==  "5" ]] ; then
            break
        fi
    done < HIST.txt
}

#main
main()
{
    createANSFile
    createHISTFile

    while [ "$num1" != "EXIT" ] ; do
        clear
        readOperation
        #operation="HIST"
        if [ "$num1" != "EXIT" ] ; then
            if [[ "$num1" == "HIST" ]] ; then
                indidate5NewestHistoryLine
                pauseUntillPressAny
            else
                processOperation
                pauseUntillPressAny
            fi
        fi
    done
}

#run the file 
main