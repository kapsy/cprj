# if [ $# -ne 1 ]; then
#     echo "Incorrect number of args!"
#     UsageExit
# fi


while getopts n: name
do
    case $name in
        n)PROJECT_NAME=$OPTARG;;
    esac
done
shift "$((OPTIND-1))"

_cprj -n $PROJECT_NAME
CPRJ_RESULT=$?
if [ $CPRJ_RESULT = 0 ]; then

    cd $PROJECT_NAME
    vim code/$PROJECT_NAME.cc

else
    exit 1
fi

exit 0
