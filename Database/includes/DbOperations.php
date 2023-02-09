<?php

    class DbOperations{
        private $con;

        function __construct()
        {
            require_once dirname(__FILE__).'/DbConnect.php';

            $db = new DbConnect();

            $this->con = $db->connect();
        }
        /*CRUD -> C ->CREATE*/ 

        function createUser($username,$email,$pass)	//changed
        {
		if($this->isUserExist($username,$email)){
		    return 0; 
		}else{
			    $password = md5($pass);
			    $stmt = $this->con->prepare("INSERT INTO `User`(`USER_ID`, `name`, `email`, `password`) VALUES (NULL, ? , ? , ?);");
			    $stmt->bind_param("sss",$username,$email,$password);

			    if($stmt->execute()){
				return 1; 
			    }else{
				return 2; 
			    }
			}
        }
        
        public function insertTripData($username,$email,$duration, $alerts, $userId){
            if($this->isUserExist($username,$email)){	//if exists
		    	$stmt = $this->con->prepare("INSERT INTO `Trip`(`TRIP_ID`, `trip_duration`, `number_of_alerts`, `USER_ID`) VALUES (NULL, ? , ? , ?);");
				$stmt->bind_param("sss", $duration, $alerts, $userId); 

				if($stmt->execute()){
					return 1; //ok
				}else{
					return 2; //error
				}
			}else{
            	return 0; //user doesnt exists
			}
		}

        public function userLogin($email, $pass){
			$password = md5($pass);
			$stmt = $this->con->prepare(
            "SELECT `USER_ID` FROM `User` WHERE `email` = ? AND `password` = ?");
			$stmt->bind_param("ss",$email,$password);
			$stmt->execute();
			$stmt->store_result(); 
			return $stmt->num_rows > 0; 
		}

		public function getUserByEmail($email){
			$stmt = $this->con->prepare("SELECT * FROM `User` WHERE `email` = ?");
			$stmt->bind_param("s",$email);
			$stmt->execute();
			return $stmt->get_result()->fetch_assoc();
		}
		

		private function isUserExist($username, $email){
			$stmt = $this->con->prepare("SELECT `USER_ID` FROM `User` WHERE `name` = ? OR `email` = ?");
			$stmt->bind_param("ss", $username, $email);
			$stmt->execute(); 
			$stmt->store_result();
			return $stmt->num_rows > 0; 
		}
		
		
		
		
    }
?>
