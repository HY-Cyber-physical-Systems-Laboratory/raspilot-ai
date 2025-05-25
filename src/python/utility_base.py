from abc import ABC, abstractmethod
from typing import Any, Dict, List, Optional, Tuple



class UtilityBase(ABC):
    """
    Abstract base class for utility classes.

    This class defines the interface for utility classes that can be used to perform various operations.
    """

    def __init__(self, name: str):
        """
        Initialize the utility class with a name.

        Args:
            name (str): The name of the utility.
        """
        self.name = name

    @abstractmethod
    def execute(self, *args: Any, **kwargs: Any) -> Any:
        """
        Execute the utility operation.

        Args:
            *args: Positional arguments for the operation.
            **kwargs: Keyword arguments for the operation.

        Returns:
            Any: The result of the operation.
        """
        pass

    @abstractmethod
    def get_info(self) -> Dict[str, Any]:
        """
        Get information about the utility.

        Returns:
            Dict[str, Any]: A dictionary containing information about the utility.
        """
        pass

    @staticmethod
    def read_msg_file(msg_file: str) -> List[str]:
        """
        Read a message file and return its contents as a list of strings.

        Args:
            msg_file (str): The path to the message file.

        Returns:
            List[str]: A list of strings containing the contents of the message file.
        """
        with open(msg_file, 'r') as file:
            return [line.strip() for line in file.readlines()]