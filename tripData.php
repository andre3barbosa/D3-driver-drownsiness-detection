<?php 

require_once '../includes/DbOperations.php';

$response = array();

if($_SERVER['REQUEST_METHOD']=='POST'){
    if(
        isset($_POST['username']) and
            isset($_POST['email']) and
                isset($_POST['duration']) and
                    isset($_POST['numAlerts']) and
                        isset($_POST['userId']) )        
        {
            //operate the data further 
            
            $db = new DbOperations();
            
            $result = $db->insertTripData(  $_POST['username'],
                                            $_POST['email'],   
                                            $_POST['duration'],
                                            $_POST['numAlerts'],
                                            $_POST['userId']
                                        );
            if($result == 1){
                $response['error'] = false; 
                $response['message'] = "Data updated successfully";
            }elseif($result == 2){
                $response['error'] = true; 
                $response['message'] = "Some error occurred please try again";			
            }elseif($result == 0){
                $response['error'] = true; 
                $response['message'] = "It seems you are not registered, please register before";						
            }

        }else{
            $response['error'] = true; 
            $response['message'] = "Required fields are missing";
        }

}else{
    $response['error'] = true;
    $response['message'] = "Invalid Request";
}

echo json_encode($response);

?>